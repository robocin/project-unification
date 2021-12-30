#ifndef PROJECT_UNIFICATION_SSLNAVIGATION_H
#define PROJECT_UNIFICATION_SSLNAVIGATION_H

#include "Packages/Packages.h"

class SSLNavigation {
  std::optional<Robot> robot;

  struct Ctes {
    static constexpr double PROP_VELOCITY_MIN_FACTOR = 0.1;        // 0 - 1
    static constexpr double MAX_VELOCITY = 2.2;                    // 0 - 5
    static constexpr double ANGLE_EPSILON = 0.1;                   // 0 - PI
    static constexpr double ANGLE_KP = 5;                          // 0 - 10
    static constexpr double ROTATE_IN_POINT_MIN_VEL_FACTOR = 0.18; // 0 - 1
    static constexpr double ROTATE_IN_POINT_APPROACH_KP = 2;       // 0 - 10
    static constexpr double ROTATE_IN_POINT_MAX_VELOCITY = 1.8;    // 0 - 5
    static constexpr double ROTATE_IN_POINT_ANGLE_KP = 5;          // 0 - 10
    static constexpr double MIN_DIST_TO_PROP_VELOCITY = 720;

    static constexpr double ADJUST_ANGLE_MIN_DIST = 50;
  };

 public:
  SSLNavigation() = default;

  inline SSLCommand run(const Robot& r, const RobotCommand& command) {
    robot = r;
    auto move = std::visit(*this, command.motion());

    SSLCommand navigation(r.id(), move.velocity(), move.angularVelocity());

    navigation.set_kickSpeed(command.kickSpeed());
    navigation.set_chip(command.chip());
    navigation.set_front(command.front());
    navigation.set_dribbler(command.dribbler());
    navigation.set_dribblerVelocity(command.dribblerVelocity());

    return navigation;
  }

  class RobotMove {
    PROPERTY(Point, velocity, Point(0, 0));
    PROPERTY(double, angularVelocity, 0);

    CTOR(RobotMove, velocity, angularVelocity);
    CTOR(RobotMove, angularVelocity);
  };

  inline RobotMove anglePID(double targetAngle, double kp) {
    auto dTheta = Geometry::smallestAngleDiff<double>(robot->angle(), targetAngle);
    return RobotMove(kp * dTheta);
  }

  inline RobotMove operator()(const Motion::GoToPoint& goToPoint) {
    // If Player send max speed, this max speed has to be respected
    // Ohterwise, use the max speed received in the parameter

    double maxVelocity = goToPoint.optional_maxVelocity().value_or(Ctes::MAX_VELOCITY);
    double distanceToGoal = robot->distTo(goToPoint.target());
    double kp = goToPoint.optional_kp().value_or(Ctes::ANGLE_KP);

    // If it is to use proportional speed and the distance to the target position is less the
    // threshold
    if (goToPoint.usingPropVelocity()) {
      double propVelocityFactor = Ctes::PROP_VELOCITY_MIN_FACTOR;
      double minPropDistance =
          goToPoint.optional_propMinDistance().value_or(Ctes::MIN_DIST_TO_PROP_VELOCITY);

      // If Player send the min prop speed factor, it has to be respected
      // Ohterwise, use defined in the parameter
      // TODO: Check if it is really necessary for the behaviors
      if (goToPoint.has_propVelocityFactor() &&
          Math::between(goToPoint.propVelocityFactor(), 0.0, 1.0)) {
        propVelocityFactor = goToPoint.propVelocityFactor();
      }

      if (distanceToGoal <= minPropDistance) {
        maxVelocity =
            maxVelocity * Math::map(distanceToGoal, 0.0, minPropDistance, propVelocityFactor, 1.0);
      }
    }

    if (distanceToGoal > Ctes::ADJUST_ANGLE_MIN_DIST) {
      // Uses an angle PID (only proportional for now), and first try to get in the right angle,
      // using only angular speed and then use linear speed to get into the point

      auto theta = (goToPoint.target() - robot->position()).angle();

      auto dTheta = Geometry::smallestAngleDiff(robot->angle(), goToPoint.targetAngle());

      // Proportional to prioritize the angle correction
      auto v_prop =
          std::abs(Geometry::smallestAngleDiff<double>(PI - Ctes::ANGLE_EPSILON, dTheta)) *
          (maxVelocity / (PI - Ctes::ANGLE_EPSILON));

      return RobotMove(Geometry2D::fromPolar<Point>(v_prop, theta), kp * dTheta);
    } else {
      return anglePID(goToPoint.targetAngle(), kp);
    }
  }

  inline RobotMove operator()(const Motion::RotateInPoint& rotatePoint) {
    double velocity =
        rotatePoint.optional_rotateVelocity().value_or(Ctes::ROTATE_IN_POINT_MAX_VELOCITY);

    if (rotatePoint.has_propVelocityFactor()) {
      velocity *= (rotatePoint.propVelocityFactor() + Ctes::ROTATE_IN_POINT_MIN_VEL_FACTOR);
    }

    double robotRadius = (robot->distTo(rotatePoint.target())) / 1000.0;
    double dtheta = Geometry::smallestAngleDiff(robot->angle(), rotatePoint.targetAngle());
    // Rotate in the smaller angle
    double clockSignal = rotatePoint.clockwise() ? 1.0 : -1.0;
    Point rotatedCoords =
        Geometry2D::rotateCCW(Point(Ctes::ROTATE_IN_POINT_APPROACH_KP *
                                        (robotRadius - rotatePoint.orbitRadius() / 1000.0),
                                    clockSignal * velocity),
                              robot->angle());

    double angVel = ((-(clockSignal * velocity) / (rotatePoint.orbitRadius() / 1000.0)) +
                     (Ctes::ROTATE_IN_POINT_ANGLE_KP * dtheta));

    return RobotMove(rotatedCoords, angVel);
  }

  RobotMove operator()(const Motion::RotateOnSelf& rotateOnSelf) {
    return anglePID(rotateOnSelf.targetAngle(),
                    rotateOnSelf.optional_kp().value_or(Ctes::ANGLE_KP));
  }
};

#endif // PROJECT_UNIFICATION_SSLNAVIGATION_H
