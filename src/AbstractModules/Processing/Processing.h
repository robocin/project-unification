//
// Created by rieijoaquim on 7/17/21.
//

#ifndef PROJECT_UNIFICATION_PROCESSING_H
#define PROJECT_UNIFICATION_PROCESSING_H

#include <soccer-common/soccer-common.h>
#include "Packages/Packages.h"

class Processing : public IndexedModuleBase {
  Q_OBJECT
 public:
  Processing(int index, QThreadPool* threadPool);

 signals:
  void sendCommand(const Command& command);
};

namespace Factory {
  inline static auto& processing = singleton_v<InheritanceFactory<Processing, int, QThreadPool*>>;
}

#endif // PROJECT_UNIFICATION_PROCESSING_H
