#ifndef PROJECT_UNIFICATION_FRAME_H
#define PROJECT_UNIFICATION_FRAME_H

#include <soccer-common/soccer-common.h>
#include "Entities/Entities.h"

class Frame {
  PROPERTY(Ball, ball);
  PROPERTY(Robots<Robot>, allies, Robots<Robot>());
  PROPERTY(Robots<Robot>, enemies, Robots<Robot>());
};

#endif // PROJECT_UNIFICATION_FRAME_H
