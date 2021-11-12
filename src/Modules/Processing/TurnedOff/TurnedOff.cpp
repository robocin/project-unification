//
// Created by jose-cruz on 06/08/2021.
//

#include "TurnedOff.h"

TurnedOff::TurnedOff(int index, QThreadPool* threadPool) : Processing(index, threadPool) {
}

void TurnedOff::exec() {
  // empty.
}

static_block {
  Factory::processing.insert<TurnedOff>();
};