//
// Created by jose-cruz on 17/07/2021.
//

#include "GrSimComm.h"

GrSimComm::GrSimComm(QThreadPool* threadPool) : Acting(threadPool) {
}

void GrSimComm::buildParameters(Parameters::Handler& parameters) {
  using namespace Parameters;

  parameters["IP"] = Text(args.simulation.ip);
  parameters["Port"] = Text(args.simulation.port);

  parameters["Replacement"]["IP"] = Text(args.replacement.ip);
  parameters["Replacement"]["Port"] = Text(args.replacement.port);
  parameters["Replacement"]["Enable"] =
      CheckBox(args.isReplacementEnabled,
               "\n  * Key P replaces the ball;\n"
               "  * Key Y/B (Y for Yellow, B for Blue) + Key [0-9] replaces the robots");
  parameters["Replacement"]["Enable"]["true"]["Robot Angle"] =
      MappedAngleInRadiansToDegrees(args.replacementAngle, -PI, PI);
}

void GrSimComm::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &Vision::sendFrame,
          this,
          &GrSimComm::receiveFrame,
          Qt::DirectConnection);

  /* keyboard and mouse replacement (connection type implicit queued). */ {
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizerBase::relativeMousePos,
            this,
            &GrSimComm::receiveRelativeMousePos);
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizerBase::onKeyPress,
            this,
            &GrSimComm::receiveKeyPress);
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizerBase::onKeyRelease,
            this,
            &GrSimComm::receiveKeyRelease);
  }
  for (Processing* processing : modules->processing()) {
    connect(processing,
            &Processing::sendCommand,
            this,
            &GrSimComm::receiveCommand,
            Qt::DirectConnection);
  }
}

void GrSimComm::init(const Modules* modules) {
}

void GrSimComm::update() {
  if (args.simulation.ip.updated() || args.simulation.port.updated()) {
    setupClient();
  }

  if (args.replacement.ip.updated() || args.replacement.port.updated()) {
    setupReplacementClient();
  }

  if (args.isReplacementEnabled) {
    if (auto ballToReplace = shared.apply(&GrSimComm::isBallReplacement)) {
      auto position = ballToReplace.value();
      auto packet = Simulation::GrSim::Replacement::ball(position, Point(0, 0));
      emit sendReplacementDatagram(Protobuf::toByteArray(packet));
    }
    if (auto robotToReplace = shared.apply(&GrSimComm::isRobotReplacement)) {
      auto [isYellowRobot, index, position] = robotToReplace.value();
      auto packet = Simulation::GrSim::Replacement::robot(isYellowRobot,
                                                          index,
                                                          args.replacementAngle,
                                                          position,
                                                          Point(0, 0));
      emit sendReplacementDatagram(Protobuf::toByteArray(packet));
    }
  }
  packets = shared->packets.get_and_clear();
  shared->frame.extract_to(frame);
}

void GrSimComm::exec() {
  if (!frame) {
    return;
  }
  if (!packets.empty()) {
    emit sendDatagram(
        Protobuf::toByteArray(Simulation::GrSim::toRobotControl(packets, frame->allies())));
    packets.clear();
  }
}

void GrSimComm::setupClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socket.reset(new QUdpSocket(this));
        qDebug() << "GrSimComm::socket:" << socket.get() << "was created";
        connect(
            this,
            &Acting::sendDatagram,
            socket.get(),
            [this,
             ip = QHostAddress(args.simulation.ip.value()),
             port = args.simulation.port.value()](const QByteArray& datagram) {
              socket->writeDatagram(datagram, ip, port);
            },
            Qt::QueuedConnection); // must be queued.
      },
      Qt::QueuedConnection); // must be queued.
}

void GrSimComm::setupReplacementClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        replacementSocket.reset(new QUdpSocket(this));
        qDebug() << "GrSimComm::replacementSocket:" << replacementSocket.get() << "was created";
        connect(
            this,
            &GrSimComm::sendReplacementDatagram,
            replacementSocket.get(),
            [this,
             ip = QHostAddress(args.replacement.ip.value()),
             port = args.replacement.port.value()](const QByteArray& datagram) {
              replacementSocket->writeDatagram(datagram, ip, port);
            },
            Qt::QueuedConnection); // must be queued.
      },
      Qt::QueuedConnection); // must be queued.
}

void GrSimComm::receiveRelativeMousePos(const Point& position) {
  shared->replacementPos = position;
}

void GrSimComm::receiveKeyPress(Qt::Key key) {
  shared->pressedKeys->insert(key);
  runInParallel();
}

void GrSimComm::receiveKeyRelease(Qt::Key key) {
  shared->pressedKeys->remove(key);
  runInParallel();
}

std::optional<Point> GrSimComm::isBallReplacement(const Shared& obj) {
  if (obj.pressedKeys->contains(Qt::Key::Key_P)) {
    return obj.replacementPos.value_or(Point(0, 0));
  } else {
    return std::nullopt;
  }
}

std::optional<std::tuple<bool, int, Point>> GrSimComm::isRobotReplacement(const Shared& obj) {
  std::optional<bool> hasIsYellow;
  if (obj.pressedKeys->contains(Qt::Key::Key_Y)) {
    hasIsYellow = true;
  } else if (obj.pressedKeys->contains(Qt::Key::Key_B)) {
    hasIsYellow = false;
  } else {
    return std::nullopt;
  }
  auto isYellow = hasIsYellow.value();
  for (int idx = 0; idx <= 9; ++idx) {
    if (obj.pressedKeys->contains(static_cast<Qt::Key>(static_cast<int>(Qt::Key::Key_0) + idx))) {
      return std::make_tuple(isYellow, idx, obj.replacementPos.value_or(Point(0, 0)));
    }
  }
  return std::nullopt;
}

void GrSimComm::receiveCommand(const Command& command) {
  if (auto* sslCommand = std::get_if<SSLCommand>(&command)) {
    shared->packets->append(*sslCommand);
    runInParallel();
  } else {
    qDebug() << "GrSimComm::receiveCommand: unknown command type.";
  }
}

void GrSimComm::receiveFrame(const Frame& frame) {
  shared->frame = frame;
}

static_block {
  Factory::acting.insert<GrSimComm>();
};
