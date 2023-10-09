#ifndef PROJECT_UNIFICATION_VSSROBOTCOMMAND_H
#define PROJECT_UNIFICATION_VSSROBOTCOMMAND_H

#include <soccer-common/soccer-common.h>

namespace VSSMotion {
  class GoToPoint {
    PROPERTY(Point, target);

  public:
    CTOR(GoToPoint, target);
  };

  class Spin {
    PROPERTY(double, power, 30.0);
    PROPERTY(bool, clockwise, false);

  public:
    CTOR(Spin, power, clockwise);
    CTOR(Spin, clockwise);
  };

  class Stop {
  public:
    CTOR(Stop);
  };

  using Type = std::variant<GoToPoint, Spin, Stop>;
} // namespace VSSMotion

class VSSRobotCommand {
  PROPERTY(VSSMotion::Type, motion);

public:
  CTOR(VSSRobotCommand, motion);
};

#endif // PROJECT_UNIFICATION_VSSROBOTCOMMAND_H
