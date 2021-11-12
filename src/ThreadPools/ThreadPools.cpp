#include "ThreadPools.h"

QThreadPool* ThreadPools::mainThreadPool() const {
  return &m_mainThreadPool;
}
