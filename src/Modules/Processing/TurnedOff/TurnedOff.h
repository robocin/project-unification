//
// Created by jose-cruz on 06/08/2021.
//

#ifndef PROJECT_UNIFICATION_TURNEDOFF_H
#define PROJECT_UNIFICATION_TURNEDOFF_H

#include "Modules/Modules.h"

class TurnedOff : public Processing {
 public:
  ~TurnedOff() override = default;

 protected:
  void exec() override;

 public:
  TurnedOff(int index, QThreadPool* threadPool);
};

#endif // PROJECT_UNIFICATION_TURNEDOFF_H
