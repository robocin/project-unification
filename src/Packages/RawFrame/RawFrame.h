//
// Created by jose-cruz on 12/06/2021.
//

#ifndef PROJECT_UNIFICATION_RAWFRAME_H
#define PROJECT_UNIFICATION_RAWFRAME_H

#include <soccer-common/soccer-common.h>
#include "Entities/Entities.h"

class RawFrame {
  PROPERTY(int, number);
  PROPERTY(int, cameraId);
  PROPERTY(QVector<RawBall>, balls, QVector<RawBall>());
  PROPERTY(Robots<RawRobot>, allies, Robots<RawRobot>());
  PROPERTY(Robots<RawRobot>, enemies, Robots<RawRobot>());
};

#endif // PROJECT_UNIFICATION_RAWFRAME_H
