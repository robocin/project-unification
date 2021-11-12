#ifndef PROJECT_UNIFICATION_MODULES_H
#define PROJECT_UNIFICATION_MODULES_H

#include "AbstractModules/AbstractModules.h"
#include "ThreadPools/ThreadPools.h"
#include "Packages/Packages.h"

class Modules : public ModulesPrivate {
  Vision* m_vision{};
  Acting* m_acting{};

  QVector<Processing*> m_processing{};

 public:
  explicit Modules(MainWindow* gui, ThreadPools* threads);
  ~Modules() override = default;

  Vision* vision() const;
  Acting* acting() const;

  const QVector<Processing*>& processing() const;
};

#endif // PROJECT_UNIFICATION_MODULES_H
