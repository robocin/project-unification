#include "GrSimReceiver.h"

GrSimReceiver::GrSimReceiver(QThreadPool* threadPool) : Vision(threadPool) {
}

void GrSimReceiver::buildParameters() {
  using namespace Parameters;

  parameters()["IP"] = Text(args.ip);
  parameters()["Port"] = Text(args.port);
  /* Network Interface */ {
    QStringList names;
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto& interface : interfaces) {
      names += interface.name();
    }
    parameters()["INet"] = ComboBox(args.inet, names);
  }
  parameters()["TeamColor"] = MappedComboBox(args.isYellow, {{false, "Blue"}, {true, "Yellow"}});
  parameters()["AttackingSide"] =
      MappedComboBox(args.isAttackingToRight, {{false, "Left"}, {true, "Right"}});
}

void GrSimReceiver::connectModules(const Modules* modules) {
}

void GrSimReceiver::init(const Modules* modules) {
  detectionKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::AboveBottom);
  fieldKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Bottom);
}

void GrSimReceiver::setupClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socket.reset(new QUdpSocket(this));
        if (socket->bind(QHostAddress::AnyIPv4, args.port, QUdpSocket::ShareAddress)) {
          qDebug() << "GrSimReceiver::socket connected on port" << args.port;
        }
        if (socket->joinMulticastGroup(QHostAddress(args.ip),
                                       QNetworkInterface::interfaceFromName(args.inet))) {
          qDebug() << "GrSimReceiver::socket connected with ip" << args.ip << "and interface"
                   << args.inet;
        }
        connect(socket.get(),
                &QUdpSocket::readyRead,
                this,
                &GrSimReceiver::receiveDatagrams,
                Qt::DirectConnection); // can be directed.
      },
      Qt::QueuedConnection); // must be queued.
}

void GrSimReceiver::receiveDatagrams() {
  std::deque<RoboCupSSL::SSL_WrapperPacket> receivedPackets;
  while (socket->hasPendingDatagrams()) {
    QByteArray datagram(static_cast<int>(socket->pendingDatagramSize()), static_cast<char>(0));
    socket->readDatagram(datagram.data(), datagram.size());
    if (auto received = Protobuf::fromByteArray<RoboCupSSL::SSL_WrapperPacket>(datagram)) {
      receivedPackets.emplace_back(*received);
    }
  }
  shared->packets.apply([&from = receivedPackets](std::deque<RoboCupSSL::SSL_WrapperPacket>& to) {
    if (to.empty()) {
      to.swap(from);
    } else {
      std::move(from.begin(), from.end(), std::back_inserter(to));
      from.clear();
    }
  });
  runInParallel();
}

void GrSimReceiver::update() {
  if (args.ip.updated() || args.port.updated() || args.inet.updated()) {
    setupClient();
  }
  if (args.isYellow.updated()) {
    emit sendIsYellow(args.isYellow);
    if (args.isYellow) {
      emit setInterfaceTeamColor(Color::Yellow, Color::Blue);
    } else {
      emit setInterfaceTeamColor(Color::Blue, Color::Yellow);
    }
  }
  if (args.isAttackingToRight.updated()) {
    emit setInterfaceAttackSide(args.isAttackingToRight);
  }
  shared->packets->swap(packets);
}

void GrSimReceiver::exec() {
  std::optional<Field> field;
  QVector<RawFrame> rawFrames;
  while (!packets.empty()) {
    auto packet = std::move(packets.front());
    packets.pop_front();
    if (packet.has_detection()) {
      const auto& detection = packet.detection();

      if (auto lossType = loss(detection)) {
        qDebug().nospace() << MagicEnum::name(lossType.value()) << " packets on cam "
                           << detection.camera_id() << ".";
      }

      RawFrame rawFrame;
      rawFrame.set_cameraId(static_cast<int>(detection.camera_id()));
      rawFrame.set_number(static_cast<int>(detection.frame_number()));
      /* getting balls info */ {
        QVector<std::pair<float, RawBall>> balls;
        for (const auto& ball : detection.balls()) {
          balls += std::pair(ball.confidence(), RawBall(Point(ball.x(), ball.y())));
        }
        /* TODO: How we can do it better? (ball confidence) */ {
          std::sort(balls.begin(), balls.end(), std::greater());
          if (!balls.empty()) {
            rawFrame.set_balls(QVector<RawBall>({balls.front().second}));
          }
        }
      }
      /* getting robots info */ {
        Robots<RawRobot> yellowRobots;
        for (const auto& robot : detection.robots_yellow()) {
          yellowRobots += RawRobot(static_cast<int>(robot.robot_id()),
                                   robot.orientation(),
                                   Point(robot.x(), robot.y()));
        }

        Robots<RawRobot> blueRobots;
        for (const auto& robot : detection.robots_blue()) {
          blueRobots += RawRobot(static_cast<int>(robot.robot_id()),
                                 robot.orientation(),
                                 Point(robot.x(), robot.y()));
        }

        if (args.isYellow) {
          rawFrame.set_allies(yellowRobots);
          rawFrame.set_enemies(blueRobots);
        } else {
          rawFrame.set_allies(blueRobots);
          rawFrame.set_enemies(yellowRobots);
        }
      }

      rawFrames += rawFrame;

      detectionDraw.update(detection);
      detectionKey.draw(detectionDraw);
    }

    if (packet.has_geometry()) {
      const auto& geometry = packet.geometry().field();

      field.emplace(args.isAttackingToRight,
                    geometry.field_length(),
                    geometry.field_width(),
                    geometry.goal_width(),
                    geometry.goal_depth(),
                    geometry.penalty_area_depth(),
                    geometry.penalty_area_width());

      fieldKey.draw(DrawSSLClientField(geometry, true));
    }
  }
  if (field) {
    emit sendField(field.value());
  }
  if (!rawFrames.empty()) {
    // emit sendRawFrames(rawFrames);
    // TODO: sendFrame??
  }
}

static_block {
  Factory::vision.insert<GrSimReceiver>("Receives camera info");
};