//
// Created by jose-cruz on 06/08/2021.
//

#ifndef PROJECT_UNIFICATION_MOUSEANDKEYBOARD_H
#define PROJECT_UNIFICATION_MOUSEANDKEYBOARD_H

#include "Modules/Modules.h"

class MouseAndKeyboard : public Processing {
 public:
  MouseAndKeyboard(int index, QThreadPool* threadPool);

 protected:
  void buildParameters() override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  struct Args {
    Parameters::Arg<Qt::Key> targetKey = Qt::Key_T;
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

  std::optional<Field> field;
  std::optional<Frame> frame;
  std::optional<Robot> robot;
  std::optional<Point> mouse;

  GameVisualizer::Key targetKey;

 private slots:
  void receiveField(const Field& field);
  void receiveFrame(const Frame& frame);
  void receiveMousePos(const Point& mouse);
  void receiveKeyPressed(Qt::Key key);
  void receiveKeyReleased(Qt::Key key);
};

#endif // PROJECT_UNIFICATION_MOUSEANDKEYBOARD_H
