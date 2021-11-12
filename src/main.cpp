#include <QApplication>
#include "Modules/Modules.h"

static constexpr int MAX_ROBOTS = 11; // considering max robots for all avaliable fields.
static constexpr QSizeF DEFAULT_VISUALIZER_SIZE = QSizeF(5, 5);

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  MainWindow gui(MAX_ROBOTS, DEFAULT_VISUALIZER_SIZE);
  ThreadPools threads;
  Modules modules(&gui, &threads);
  gui.setup(ROBOCIN_PROJECT_NAME, ROBOCIN_PROJECT_VERSION, ROBOCIN_PROJECT_PATH);
  gui.show();
  InterruptRequest<SIGINT>::setup([&gui]() {
    QMetaObject::invokeMethod(&gui, &MainWindow::close, Qt::QueuedConnection); // must be queued.
  });
  if (QApplication::arguments().contains("-running")) {
    QMetaObject::invokeMethod(
        &gui,
        [&gui]() {
          gui.playPauseButton()->setRunning(true);
        },
        Qt::QueuedConnection);
  }
  return QApplication::exec();
}
