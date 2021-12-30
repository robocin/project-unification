//
// Created by jose-cruz on 06/08/2021.
//

#ifndef PROJECT_UNIFICATION_MOUSEANDKEYBOARD_H
#define PROJECT_UNIFICATION_MOUSEANDKEYBOARD_H

#include "Modules/Modules.h"
#include "Modules/Processing/ProcessingUtils/ProcessingUtils.h"

class MouseAndKeyboard : public Processing {
 public:
  MouseAndKeyboard(int index, QThreadPool* threadPool);

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
    enum class League { SSL, VSS };
    Parameters::Arg<League> league = League::SSL;
    struct SSL {
      Parameters::Arg<Qt::Key> goToPointKey = Qt::Key_T;
      Parameters::Arg<Qt::Key> rotateInPointKey = Qt::Key_U;
      Parameters::Arg<Qt::Key> rotateOnSelfKey = Qt::Key_I;
    };

    struct VSS {
      Parameters::Arg<Qt::Key> goToPointKey = Qt::Key_T;
      Parameters::Arg<Qt::Key> ccwSpin = Qt::Key_Z;
      Parameters::Arg<Qt::Key> cwSpin = Qt::Key_X;
    };

    SSL ssl;
    VSS vss;
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

  SSLNavigation sslNavigation;
  VSSNavigation vssNavigation;

 private slots:
  void receiveField(const Field& field);
  void receiveFrame(const Frame& frame);
  void receiveMousePos(const Point& mouse);
  void receiveKeyPressed(Qt::Key key);
  void receiveKeyReleased(Qt::Key key);
};

#endif // PROJECT_UNIFICATION_MOUSEANDKEYBOARD_H
