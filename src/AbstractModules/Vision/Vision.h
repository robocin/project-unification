#ifndef PROJECT_UNIFICATION_VISION_H
#define PROJECT_UNIFICATION_VISION_H

#include <soccer-common/soccer-common.h>
#include "Packages/Packages.h"
#include "Field/Field.h"

class Vision : public ModuleBase {
  Q_OBJECT
 public:
  explicit Vision(QThreadPool* threadPool);

 signals:
  void sendIsYellow(bool isYellow);
  void sendField(const Field& field);
  void sendFrame(const Frame& frame);
};

namespace Factory {
  inline static auto& vision = singleton_v<InheritanceFactory<Vision, QThreadPool*>>;
}

#endif // PROJECT_UNIFICATION_VISION_H
