//
// Created by jose-cruz on 08/02/2021.
//

#include "FIRASimReceiver.h"
#include "Packages/Packages.h"

FIRASimReceiver::FIRASimReceiver(QThreadPool* threadPool) : Vision(threadPool) {
}

void FIRASimReceiver::buildParameters() {
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
  parameters()["AttackSide"] =
      MappedComboBox(args.isAttackingToRight, {{false, "Left"}, {true, "Right"}});

  parameters()["Draw"]["Field Markings"] = CheckBox(args.drawFieldMarkings);
  parameters()["Draw"]["CIninho"] = CheckBox(args.drawCIninho);
}

void FIRASimReceiver::connectModules(const Modules* modules) {
}

void FIRASimReceiver::init(const Modules* modules) {
  detectionKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::AboveBottom);
  fieldKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Bottom);
}

void FIRASimReceiver::setupClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socket.reset(new QUdpSocket(this));
        if (socket->bind(QHostAddress::AnyIPv4, args.port, QUdpSocket::ShareAddress)) {
          qDebug() << "FIRASimReceiver::socket connected on port" << args.port;
        }
        if (socket->joinMulticastGroup(QHostAddress(args.ip),
                                       QNetworkInterface::interfaceFromName(args.inet))) {
          qDebug() << "FIRASimReceiver::socket connected with ip" << args.ip << "and interface"
                   << args.inet;
        }
        connect(socket.get(),
                &QUdpSocket::readyRead,
                this,
                &FIRASimReceiver::receiveDatagram,
                Qt::DirectConnection);
      },
      Qt::QueuedConnection);
}

void FIRASimReceiver::receiveDatagram() {
  using EnvironmentPacketType = fira_message::sim_to_ref::Environment;

  std::deque<EnvironmentPacketType> receivedPackets;
  while (socket->hasPendingDatagrams()) {
    QByteArray datagram(static_cast<int>(socket->pendingDatagramSize()), static_cast<char>(0));
    socket->readDatagram(datagram.data(), datagram.size());
    if (auto packet = Protobuf::fromByteArray<EnvironmentPacketType>(datagram)) {
      receivedPackets.emplace_back(std::move(packet.value()));
    }
  }
  shared->packets.apply([&from = receivedPackets](std::deque<EnvironmentPacketType>& to) {
    if (to.empty()) {
      to.swap(from);
    } else {
      std::move(from.begin(), from.end(), std::back_inserter(to));
      from.clear();
    }
  });
  runInParallel();
}

void FIRASimReceiver::update() {
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
  shared->packets->swap(packets); // local in this point is empty, taking all received from shared.
}

void FIRASimReceiver::exec() {
  static constexpr double CM_SCALE = 100.0;
  Frame frame;
  while (!packets.empty()) {
    auto environment = std::move(packets.front());
    packets.pop_front();
    if (environment.has_frame()) {
      const auto& f = environment.frame();

      Robots<Robot> blueRobots;
      for (const auto& robot : f.robots_blue()) {
        blueRobots += Robot(static_cast<int>(robot.robot_id()),
                            static_cast<float>(robot.orientation()),
                            QPointF(robot.x(), robot.y()) * CM_SCALE,
                            QPointF(robot.vx(), robot.vy()) * CM_SCALE,
                            QPointF(0, 0) * CM_SCALE); // TODO: acceleration
      }

      Robots<Robot> yellowRobots;
      for (const auto& robot : f.robots_yellow()) {
        yellowRobots += Robot(static_cast<int>(robot.robot_id()),
                              static_cast<float>(robot.orientation()),
                              QPointF(robot.x(), robot.y()) * CM_SCALE,
                              QPointF(robot.vx(), robot.vy()) * CM_SCALE,
                              QPointF(0, 0) * CM_SCALE); // TODO: acceleration
      }
      if (args.isYellow) {
        frame.emplace_allies(yellowRobots);
        frame.emplace_enemies(blueRobots);
      } else {
        frame.emplace_allies(blueRobots);
        frame.emplace_enemies(yellowRobots);
      }

      Ball ball(QPointF(f.ball().x(), f.ball().y()) * CM_SCALE,
                QPointF(f.ball().vx(), f.ball().vy()) * CM_SCALE,
                QPointF(0, 0) * CM_SCALE); // TODO: acceleration
      frame.set_ball(ball);
    }
    if (environment.has_field()) {
      const auto& f = environment.field();
      static constexpr double GOAL_AREA_DEPTH = 15; // 15cm
      static constexpr double GOAL_AREA_WIDTH = 70; // 70cm
      field.emplace(args.isAttackingToRight,
                    f.length() * CM_SCALE,
                    f.width() * CM_SCALE,
                    f.goal_depth() * CM_SCALE,
                    f.goal_width() * CM_SCALE,
                    GOAL_AREA_DEPTH,
                    GOAL_AREA_WIDTH);
      emit sendField(field.value());
      fieldKey.draw(DrawField(field.value(), args.drawFieldMarkings, args.drawCIninho));
    }
  }
  if (!frame.allies().empty() || !frame.enemies().empty() || frame.has_ball()) {
    emit sendFrame(frame);
    const auto& yellowRobots = args.isYellow ? frame.allies() : frame.enemies();
    const auto& blueRobots = args.isYellow ? frame.enemies() : frame.allies();
    drawGameContext.update(frame.ball(), blueRobots, yellowRobots);
    detectionKey.draw(drawGameContext);
  }
}

static_block {
  Factory::vision.insert<FIRASimReceiver>("");
};
