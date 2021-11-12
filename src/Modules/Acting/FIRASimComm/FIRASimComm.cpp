//
// Created by jose-cruz on 09/02/2021.
//

#include "FIRASimComm.h"

FIRASimComm::FIRASimComm(QThreadPool* threadPool) : Acting(threadPool) {
}

void FIRASimComm::buildParameters() {
  using namespace Parameters;

  parameters()["IP"] = Text(args.ip);
  parameters()["Port"] = Text(args.port);
  parameters()["Replacement"]["Enable"] =
      CheckBox(args.isReplacementEnabled,
               "\n  * Key P replaces the ball;\n"
               "  * Key Y/B (Y for Yellow, B for Blue) + Key [0-9]");
  parameters()["Replacement"]["Enable"]["true"]["Robot Angle"] =
      MappedAngleInRadiansToDegrees(args.replacementAngle, -PI, PI);
}

void FIRASimComm::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &Vision::sendField,
          this,
          &FIRASimComm::receiveField,
          Qt::DirectConnection);
  connect(modules->vision(),
          &Vision::sendFrame,
          this,
          &FIRASimComm::receiveFrame,
          Qt::DirectConnection);
  connect(modules->vision(),
          &Vision::sendIsYellow,
          this,
          &FIRASimComm::receiveIsYellow,
          Qt::DirectConnection);

  for (auto processing : modules->processing()) {
    connect(processing,
            &Processing::sendCommand,
            this,
            &FIRASimComm::receiveCommand,
            Qt::DirectConnection);
  }

  /* keyboard and mouse replacement (connection type implicit queued). */ {
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizer::relativeMousePos,
            this,
            &FIRASimComm::receiveRelativeMousePos);
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizer::onKeyPress,
            this,
            &FIRASimComm::receiveKeyPress);
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizer::onKeyRelease,
            this,
            &FIRASimComm::receiveKeyRelease);
  }
}

void FIRASimComm::init(const Modules* modules) {
}

void FIRASimComm::update() {
  if (args.ip.updated() || args.port.updated()) {
    setupClient();
  }
  if (args.isReplacementEnabled) {
    if (auto ballToReplace = shared.apply(&FIRASimComm::isBallReplacement)) {
      auto position = ballToReplace.value();
      auto packet = Simulation::FIRASim::Replacement::ball(position, Point(0, 0));
      emit sendDatagram(Protobuf::toByteArray(packet));
    }
    if (auto robotToReplace = shared.apply(&FIRASimComm::isRobotReplacement)) {
      auto [isYellowRobot, index, position] = robotToReplace.value();
      auto packet = Simulation::FIRASim::Replacement::robot(isYellowRobot,
                                                            index,
                                                            args.replacementAngle,
                                                            position,
                                                            Point(0, 0));
      emit sendDatagram(Protobuf::toByteArray(packet));
    }
  }
  shared->frame.extract_to(frame);
  shared->field.extract_to(field);
  shared->hasIsYellow.extract_to(hasIsYellow);
  shared->commands->swap(commands); // swap because local always start empty.
}

void FIRASimComm::exec() {
  if (!field.has_value() || !frame.has_value() || !hasIsYellow.has_value()) {
    return;
  }
  if (!commands.empty()) {
    bool isYellow = hasIsYellow.value();
    emit sendDatagram(
        Protobuf::toByteArray(Simulation::FIRASim::toCommunicationPacket(commands, isYellow)));
    commands.clear();
  }
}

void FIRASimComm::setupClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socket.reset(new QUdpSocket(this));
        qDebug() << "FIRASimComm::socket:" << socket.get() << "was created";
        connect(
            this,
            &Acting::sendDatagram,
            socket.get(),
            [this, ip = QHostAddress(args.ip.value()), port = args.port.value()](
                const QByteArray& datagram) {
              socket->writeDatagram(datagram, ip, port);
            },
            Qt::QueuedConnection); // must be queued.
      },
      Qt::QueuedConnection); // must be queued.
}

void FIRASimComm::receiveIsYellow(bool value) {
  shared->hasIsYellow.set(value);
}

void FIRASimComm::receiveField(const Field& package) {
  shared->field.set(package);
}

void FIRASimComm::receiveFrame(const Frame& package) {
  shared->frame.set(package);
  runInParallel();
}

void FIRASimComm::receiveCommand(const Command& command) {
  if (auto* vssCommand = std::get_if<VSSCommand>(&command)) {
    shared->commands->append(*vssCommand);
    runInParallel();
  } else {
    qDebug() << "FIRASimComm::receiveCommand: unknown command.";
  }
}

void FIRASimComm::receiveRelativeMousePos(const Point& position) {
  shared->replacementPos.set(position);
}

void FIRASimComm::receiveKeyPress(Qt::Key key) {
  shared->pressedKeys->insert(key);
  runInParallel();
}

void FIRASimComm::receiveKeyRelease(Qt::Key key) {
  shared->pressedKeys->remove(key);
  runInParallel();
}

std::optional<Point> FIRASimComm::isBallReplacement(const Shared& obj) {
  if (obj.pressedKeys->contains(Qt::Key::Key_P)) {
    return obj.replacementPos.value_or(Point(0, 0));
  } else {
    return std::nullopt;
  }
}

std::optional<std::tuple<bool, int, Point>> FIRASimComm::isRobotReplacement(const Shared& obj) {
  std::optional<bool> optIsYellow;
  if (obj.pressedKeys->contains(Qt::Key::Key_Y)) {
    optIsYellow = true;
  } else if (obj.pressedKeys->contains(Qt::Key::Key_B)) {
    optIsYellow = false;
  } else {
    return std::nullopt;
  }
  auto isYellow = optIsYellow.value();
  for (int idx = 0; idx <= 9; ++idx) {
    if (obj.pressedKeys->contains(static_cast<Qt::Key>(static_cast<int>(Qt::Key::Key_0) + idx))) {
      return std::make_tuple(isYellow, idx, obj.replacementPos.value_or(Point(0, 0)));
    }
  }
  return std::nullopt;
}

static_block {
  Factory::acting.insert<FIRASimComm>("");
};
