#ifndef PROJECT_UNIFICATION_THREADPOOLS_H
#define PROJECT_UNIFICATION_THREADPOOLS_H

#include <QThreadPool>

class ThreadPools {
  mutable QThreadPool m_mainThreadPool;

 public:
  ThreadPools() = default;
  QThreadPool* mainThreadPool() const;
};

#endif // PROJECT_UNIFICATION_THREADPOOLS_H
