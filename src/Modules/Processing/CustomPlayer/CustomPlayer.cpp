#include "CustomPlayer.h"
#include "Packages/SSLRobotCommand/SSLRobotCommand.h"

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
  if (!field || !frame || !robot) {
    return;
  }
  if(!frame->has_ball()) return;
  auto&& pos_ball = frame->ball();
  if(field->enemyPenaltyAreaContains(pos_ball)) return;
  if(robot->distTo(pos_ball)>150)
  {
    SSLMotion::GoToPoint goToPoint(pos_ball,
                                   (pos_ball - robot->position()).angle(),
                                   true);
    auto command = sslNavigation.run(robot.value(), SSLRobotCommand(goToPoint));
    command.set_dribbler(true);
    command.set_dribblerVelocity(4);
    emit sendCommand(command);
  }else {
    SSLMotion::RotateOnSelf Rotate((field->enemyGoalInsideCenter() -robot->position()).angle());
    auto command = sslNavigation.run(robot.value(), SSLRobotCommand(Rotate));
    command.set_dribbler(true);
    command.set_dribblerVelocity(4);
    emit sendCommand(command);
    if(abs(robot->angleTo(field->enemyGoalInsideCenter()))<0.1)
    {
      SSLMotion::GoToPoint goToPoint(pos_ball,
        (pos_ball - robot->position()).angle(),
        true);
      auto irparapos = sslNavigation.run(robot.value(), SSLRobotCommand(goToPoint));
      irparapos.set_front(true);
      irparapos.set_kickSpeed(2);

      if(robot->distTo(field->enemyGoalInsideCenter())<4000)
      {
        irparapos.set_kickSpeed(6);
      }
      emit sendCommand(irparapos);
    }

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
