#ifndef SSL_UNIFICATION_ROBOTCOMMAND_H
#define SSL_UNIFICATION_ROBOTCOMMAND_H

#include <soccer-common/soccer-common.h>

namespace Motion {
  class GoToPoint {
    PROPERTY(Point, target);
    PROPERTY(double, targetAngle);
    PROPERTY(double, maxVelocity);
    PROPERTY(double, kp);
    PROPERTY(double, propVelocityFactor);
    PROPERTY(double, propMinDistance);
    PROPERTY(bool, usingPropVelocity, false);

    CTOR(GoToPoint, target, targetAngle);
    CTOR(GoToPoint, target, targetAngle, usingPropVelocity);
  };

  class RotateInPoint {
    PROPERTY(Point, target);
    PROPERTY(double, targetAngle);
    PROPERTY(bool, clockwise);
    PROPERTY(double, rotateVelocity);
    PROPERTY(double, orbitRadius);
    PROPERTY(double, propVelocityFactor);

    CTOR(RotateInPoint, target, targetAngle, clockwise, orbitRadius);
    CTOR(RotateInPoint, target, targetAngle, clockwise, orbitRadius, propVelocityFactor);
  };

  class RotateOnSelf {
    PROPERTY(double, targetAngle);
    PROPERTY(double, kp);

    CTOR(RotateOnSelf, targetAngle);
  };

  using Type = std::variant<GoToPoint, RotateInPoint, RotateOnSelf>;
} // namespace Motion

class RobotCommand {
  PROPERTY(double, kickSpeed, 0);
  PROPERTY(bool, front, false);
  PROPERTY(bool, chip, false);
  PROPERTY(bool, dribbler, false);
  PROPERTY(double, dribblerVelocity, 0);
  PROPERTY(Motion::Type, motion);

  CTOR(RobotCommand, motion);
};

#endif // SSL_UNIFICATION_ROBOTCOMMAND_H
