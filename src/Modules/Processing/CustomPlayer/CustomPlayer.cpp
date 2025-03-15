#include "CustomPlayer.h"

CustomPlayer::CustomPlayer(int index, QThreadPool* threadPool) : Processing(index, threadPool) {
}

void CustomPlayer::buildParameters(Parameters::Handler& parameters) {
}

void CustomPlayer::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &Vision::sendFrame,
          this,
          &CustomPlayer::receiveFrame,
          Qt::DirectConnection);

  connect(modules->vision(),
          &Vision::sendField,
          this,
          &CustomPlayer::receiveField,
          Qt::DirectConnection);
}

void CustomPlayer::init(const Modules* modules) {
}

void CustomPlayer::update() {
  shared->field.extract_to(field);
  if (auto f = shared->frame.get_optional_and_reset()) {
    if (auto it = f->allies().findById(index()); it != f->allies().end()) {
      robot = *it;
    }
    frame.emplace(*f);
  }
}

void CustomPlayer::exec() {

  if (!field || !frame || !robot)
    return;
  if (!frame->has_ball())
    return;
  auto&& pos_ball = frame->ball();

  QPointF targetPosition(1000, 4000);
  QPointF initialPosition(2000, 0);
  int tolerance = 150;
  maxdis = 9000000;
  if ((pos_ball.distTo(targetPosition)) > tolerance) {

    for (const auto& ally : frame->allies()) {
      double distance = ally.distTo(pos_ball);
      if (maxdis > distance) {
        maxdis = distance;
        idrobot = ally.id();
      }
    }
  }

  if (robot->id() != idrobot)
    return;

  if ((pos_ball - targetPosition).manhattanLength() > tolerance) {
    if (robot->distTo(pos_ball) > 150) {
      SSLMotion::GoToPoint goToPoint(pos_ball, (pos_ball - robot->position()).angle(), true);
      auto command = sslNavigation.run(robot.value(), SSLRobotCommand(goToPoint));
      command.set_dribbler(true);
      command.set_dribblerVelocity(4);
      emit sendCommand(command);
    } else {
      SSLMotion::GoToPoint goToPoint(targetPosition,
                                     (targetPosition - robot->position()).angle(),
                                     true);
      goToPoint.set_maxVelocity(1);
      auto command = sslNavigation.run(robot.value(), SSLRobotCommand(goToPoint));
      command.set_dribbler(true);
      command.set_dribblerVelocity(4);
      emit sendCommand(command);
    }
  } else if (robot->id() == idrobot && robot->distTo(initialPosition) > tolerance) {
    SSLMotion::GoToPoint goToInitial(initialPosition,
                                     (initialPosition - robot->position()).angle(),
                                     true);
    auto command = sslNavigation.run(robot.value(), SSLRobotCommand(goToInitial));
    emit sendCommand(command);
  } else {
    idrobot = -1;
    maxdis = 9000000;
  }
}

void CustomPlayer::receiveField(const Field& field) {
  shared->field = field;
}

void CustomPlayer::receiveFrame(const Frame& frame) {
  shared->frame = frame;
  runInParallel();
}

static_block {
  Factory::processing.insert<CustomPlayer>();
};
