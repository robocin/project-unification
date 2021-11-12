//
// Created by jose-cruz on 06/08/2021.
//

#ifndef PROJECT_UNIFICATION_COMMAND_H
#define PROJECT_UNIFICATION_COMMAND_H

#include <soccer-common/soccer-common.h>

class SSLCommand {
  PROPERTY(int, id);
  PROPERTY(Point, velocity);
  PROPERTY(float, angularVelocity);

  CTOR(SSLCommand, id, velocity, angularVelocity);
};

class VSSCommand {
  PROPERTY(int, id);
  PROPERTY(float, leftWheel, 0);
  PROPERTY(float, rightWheel, 0);

  CTOR(VSSCommand, id, leftWheel, rightWheel);
};

using Command = std::variant<SSLCommand, VSSCommand>;

#endif // PROJECT_UNIFICATION_COMMAND_H
