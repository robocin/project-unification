#ifndef PROJECT_UNIFICATION_VSSNAVIGATION_H
#define PROJECT_UNIFICATION_VSSNAVIGATION_H

#include "Packages/Packages.h"

class VSSNavigation {
  std::optional<Robot> robot;

  struct Ctes {
    static constexpr double KP = 20.0;
    static constexpr double KD = 2.5;
  };

  double lastError = 0;

 public:
  inline VSSCommand run(const Robot& r, const VSSRobotCommand& command) {
    robot = r;
    return std::visit(*this, command.motion());
  }

  inline VSSCommand operator()(const VSSMotion::GoToPoint& goToPoint) {
    return pid(goToPoint.target());
  }

  inline VSSCommand operator()(const VSSMotion::Spin& spin) {
    const auto [left, right] = spin.clockwise() ? std::pair{-1, +1} : std::pair{+1, -1};
    return VSSCommand(robot->id(), spin.power() * left, spin.power() * right);
  }

  inline VSSCommand operator()(const VSSMotion::Stop&) {
    return VSSCommand::halt(robot->id());
  }

  inline VSSCommand pid(Point target) {
    bool reversed = false;

    double angle_rob = robot->angle();
    double angle_obj = (target - robot->position()).angle();
    double error = Geometry::smallestAngleDiff(angle_obj, angle_rob);

    if (std::abs(error) > PI / 2.0 + PI / 20.0) {
      reversed = true;
      angle_rob = Geometry::normalizeInPI(angle_rob + PI);
      // Calculates the error and reverses the front of the robot
      error = Geometry::smallestAngleDiff(angle_obj, angle_rob);
    }

    double motorSpeed = (Ctes::KP * error) + (Ctes::KD * (error - lastError)); // + 0.2 * sumErr;
    lastError = error;

    double baseSpeed = 30;

    // Normalize
    motorSpeed = motorSpeed > 30 ? 30 : motorSpeed;
    motorSpeed = motorSpeed < -30 ? -30 : motorSpeed;

    double rightMotorSpeed;
    double leftMotorSpeed;

    if (motorSpeed > 0) {
      leftMotorSpeed = baseSpeed;
      rightMotorSpeed = baseSpeed - motorSpeed;
    } else {
      leftMotorSpeed = baseSpeed + motorSpeed;
      rightMotorSpeed = baseSpeed;
    }

    if (reversed) {
      if (motorSpeed > 0) {
        leftMotorSpeed = -baseSpeed + motorSpeed;
        rightMotorSpeed = -baseSpeed;
      } else {
        leftMotorSpeed = -baseSpeed;
        rightMotorSpeed = -baseSpeed - motorSpeed;
      }
    }
    return VSSCommand(robot->id(), leftMotorSpeed, rightMotorSpeed);
  }
};

#endif // PROJECT_UNIFICATION_VSSNAVIGATION_H
