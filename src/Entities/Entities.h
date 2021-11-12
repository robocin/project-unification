//
// Created by jose-cruz on 12/06/2021.
//

#ifndef PROJECT_UNIFICATION_ENTITIES_H
#define PROJECT_UNIFICATION_ENTITIES_H

#include <soccer-common/soccer-common.h>
#include "Field/Field.h"

using RawEntity = Common::RawEntity<Point>;
using Entity = Common::Entity<Point>;

using RawBall = Common::RawBall<Point>;
using Ball = Common::Ball<Point>;

using RawRobot = Common::RawRobot<Point>;
using Robot = Common::Robot<Point>;

template <class R>
using Robots = Common::Robots<R>;

#endif // PROJECT_UNIFICATION_ENTITIES_H
