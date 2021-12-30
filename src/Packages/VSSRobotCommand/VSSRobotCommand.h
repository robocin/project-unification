#ifndef PROJECT_UNIFICATION_VSSROBOTCOMMAND_H
#define PROJECT_UNIFICATION_VSSROBOTCOMMAND_H

#include <soccer-common/soccer-common.h>

namespace VSSMotion {
  class GoToPoint {
    PROPERTY(Point, target);

    CTOR(GoToPoint, target);
  };

  class Spin {
    PROPERTY(double, power, 30.0);
    PROPERTY(bool, clockwise, false);

    CTOR(Spin, power, clockwise);
    CTOR(Spin, clockwise);
  };

  class Stop {
    CTOR(Stop);
  };

  using Type = std::variant<GoToPoint, Spin, Stop>;
} // namespace VSSMotion

class VSSRobotCommand {
  PROPERTY(VSSMotion::Type, motion);

  CTOR(VSSRobotCommand, motion);
};

#endif // PROJECT_UNIFICATION_VSSROBOTCOMMAND_H
