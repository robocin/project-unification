#include "Modules.h"

Modules::Modules(MainWindow* gui, ThreadPools* threads) : ModulesPrivate(gui) {
  makeModule(this, m_vision, Factory::vision, threads->mainThreadPool());
  makeModule(this, m_acting, Factory::acting, threads->mainThreadPool());
  makeModule(this, m_processing, Factory::processing, threads->mainThreadPool());
}

Vision* Modules::vision() const {
  return m_vision;
}

Acting* Modules::acting() const {
  return m_acting;
}

const QVector<Processing*>& Modules::processing() const {
  return m_processing;
}