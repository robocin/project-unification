//
// Created by matheus on 17/07/2021.
//

#ifndef PROJECT_UNIFICATION_ACTING_H
#define PROJECT_UNIFICATION_ACTING_H

#include <soccer-common/soccer-common.h>

class Acting : public ModuleBase {
  Q_OBJECT
 public:
  explicit Acting(QThreadPool* threadPool);

 signals:
  void sendDatagram(const QByteArray& datagram);
};

namespace Factory {
  inline static auto& acting = singleton_v<InheritanceFactory<Acting, QThreadPool*>>;
}

#endif // PROJECT_UNIFICATION_ACTING_H
