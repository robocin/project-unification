//
// Created by jose-cruz on 06/08/2021.
//

#ifndef PROJECT_UNIFICATION_JOYSTICK_H
#define PROJECT_UNIFICATION_JOYSTICK_H

#include "Modules/Modules.h"
#include "Modules/Processing/ProcessingUtils/ProcessingUtils.h"
#include <SDL2/SDL.h>

class Joystick : public Processing {
 public:
  Joystick(int index, QThreadPool* threadPool);

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

  void ssl();
  void vss();

 private:
  struct Args {
    Parameters::Arg<int> joystickIndex = -1;
  };
  Args args;

  struct Shared {
    SharedOptional<Frame> frame;
    SharedOptional<Robot> robot;
    SharedOptional<Point> mouse;
    SharedOptional<Field> field;
    SharedValue<QSet<Qt::Key>> keys;
  };
  SharedWrapper<Shared, std::mutex> shared;

  SDL_Joystick* joystick = nullptr;

  std::optional<Field> field;
  std::optional<Frame> frame;
  std::optional<Robot> robot;
  std::optional<Point> mouse;

  GameVisualizer::Key targetKey;

  Point target{0, 0};

  SSLNavigation sslNavigation;
  VSSNavigation vssNavigation;

 private slots:
  void receiveField(const Field& field);
  void receiveFrame(const Frame& frame);
  void receiveMousePos(const Point& mouse);
  void receiveKeyPressed(Qt::Key key);
  void receiveKeyReleased(Qt::Key key);
};

#endif // PROJECT_UNIFICATION_JOYSTICK_H
