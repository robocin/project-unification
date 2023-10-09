//
// Created by jose-cruz on 06/08/2021.
//

#ifndef PROJECT_UNIFICATION_COMMAND_H
#define PROJECT_UNIFICATION_COMMAND_H

#include <soccer-common/soccer-common.h>

class SSLCommand {
  PROPERTY(int, id);
  PROPERTY(Point, velocity, Point(0, 0)); // vx, vy
  PROPERTY(double, angularVelocity, 0);   // vw
  PROPERTY(double, kickSpeed, 0);
  PROPERTY(bool, front, false);
  PROPERTY(bool, chip, false);
  PROPERTY(bool, dribbler, false);
  PROPERTY(double, dribblerVelocity, 0);

 public:
  CTOR(SSLCommand, id);
  CTOR(SSLCommand, id, angularVelocity);
  CTOR(SSLCommand, id, velocity, angularVelocity);

  static inline SSLCommand halt(int id) {
    return SSLCommand(id);
  }
};

class VSSCommand {
  PROPERTY(int, id);
  PROPERTY(double, leftWheel, 0);
  PROPERTY(double, rightWheel, 0);

 public:
  CTOR(VSSCommand, id);
  CTOR(VSSCommand, id, leftWheel, rightWheel);

  static inline VSSCommand halt(int id) {
    return VSSCommand(id);
  }
};

using Command = std::variant<SSLCommand, VSSCommand>;

#endif // PROJECT_UNIFICATION_COMMAND_H
