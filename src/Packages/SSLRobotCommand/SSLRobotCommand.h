#ifndef PROJECT_UNIFICATION_SSLROBOTCOMMAND_H
#define PROJECT_UNIFICATION_SSLROBOTCOMMAND_H

#include <soccer-common/soccer-common.h>

namespace SSLMotion {
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
} // namespace SSLMotion

class SSLRobotCommand {
  PROPERTY(double, kickSpeed, 0);
  PROPERTY(bool, front, false);
  PROPERTY(bool, chip, false);
  PROPERTY(bool, dribbler, false);
  PROPERTY(double, dribblerVelocity, 0);
  PROPERTY(SSLMotion::Type, motion);

  CTOR(SSLRobotCommand, motion);
};

#endif // PROJECT_UNIFICATION_SSLROBOTCOMMAND_H
