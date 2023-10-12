//
// Created by fbm2 on 12/10/2023.
//

#include "Joystick.h"

Joystick::Joystick(int index, QThreadPool* threadPool) : Processing(index, threadPool) {
}

void Joystick::buildParameters(Parameters::Handler& parameters) {
  qDebug() << SDL_Init(SDL_INIT_JOYSTICK);
  QMap<int, QString> joystickMap{{-1, "None"}};
  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    joystickMap.insert(i, SDL_JoystickNameForIndex(i));
  }
  parameters["Joystick"] = Parameters::MappedComboBox(args.joystickIndex, joystickMap);
}

void Joystick::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &Vision::sendFrame,
          this,
          &Joystick::receiveFrame,
          Qt::DirectConnection);

  connect(modules->vision(),
          &Vision::sendField,
          this,
          &Joystick::receiveField,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::relativeMousePos,
          this,
          &Joystick::receiveMousePos,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::onKeyPress,
          this,
          &Joystick::receiveKeyPressed,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::onKeyRelease,
          this,
          &Joystick::receiveKeyReleased,
          Qt::DirectConnection);
}

void Joystick::init(const Modules* modules) {
  targetKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Top);
}

void Joystick::update() {
  if (args.joystickIndex.updated()) {
    if (joystick == nullptr) {
      joystick = SDL_JoystickOpen(args.joystickIndex.value());
    } else {
      SDL_JoystickClose(joystick);
      joystick = SDL_JoystickOpen(args.joystickIndex.value());
    }
  }
  shared->field.extract_to(field);
  if (auto f = shared->frame.get_optional_and_reset()) {
    if (auto it = f->allies().findById(index()); it != f->allies().end()) {
      robot = *it;
    }
    frame.emplace(*f);
  }
  //   shared->mouse.extract_to(mouse);

  //   if (args.league.updated()) {
  //     targetKey.clear();
  //   }
}

void Joystick::exec() {
  if (!field || !frame || !robot || joystick == nullptr) {
    return;
  }
  SDL_JoystickUpdate();
  target = robot->position() + Point((SDL_JoystickGetAxis(joystick, 0) / 32768.0) * 20.,
                                     (SDL_JoystickGetAxis(joystick, 1) / -32768.0) * 20.);
  vss();

  // //   if (args.league == Args::League::SSL) {
  // //     ssl();
  // //   } else {
  // //   }
}

void Joystick::ssl() {
  // targetKey.draw([mouse = this->mouse](GameVisualizerPainter2D* f) {
  //   f->drawFilledCircle(mouse.value(), 45, Color::Red);
  // });

  // const QSet<Qt::Key> keys = shared->keys.get();

  // if (keys.contains(args.ssl.goToPointKey)) {
  //   SSLMotion::GoToPoint goToPoint(mouse.value(),
  //                                  (field->center() - robot->position()).angle(),
  //                                  true);
  //   auto command = sslNavigation.run(robot.value(), SSLRobotCommand(goToPoint));
  //   emit sendCommand(command);
  // } else if (keys.contains(args.ssl.rotateInPointKey)) {
  //   SSLMotion::RotateInPoint rotateInPoint(mouse.value(),
  //                                          (mouse.value() - robot->position()).angle(),
  //                                          false,
  //                                          360);
  //   auto command = sslNavigation.run(robot.value(), SSLRobotCommand(rotateInPoint));
  //   emit sendCommand(command);
  // } else if (keys.contains(args.ssl.rotateOnSelfKey)) {
  //   SSLMotion::RotateOnSelf rotateOnSelf((mouse.value() - robot->position()).angle());
  //   auto command = sslNavigation.run(robot.value(), SSLRobotCommand(rotateOnSelf));
  //   emit sendCommand(command);
  // } else {
  //   emit sendCommand(SSLCommand::halt(index()));
  // }
}

void Joystick::vss() {
  targetKey.draw([target = this->target](GameVisualizerPainter2D* f) {
    f->drawFilledCircle(target, 1.0, Color::Red);
  });

  if (target.distTo(robot->position()) < 3) {
    emit sendCommand(VSSCommand::halt(index()));
    return;
  } else {
    VSSMotion::GoToPoint goToPoint(target);
    auto command = vssNavigation.run(robot.value(), VSSRobotCommand(goToPoint));
    emit sendCommand(command);
  }
}

void Joystick::receiveField(const Field& field) {
  shared->field = field;
}

void Joystick::receiveFrame(const Frame& frame) {
  shared->frame = frame;
  runInParallel();
}

void Joystick::receiveMousePos(const Point& mouse) {
  shared.apply([&mouse](Shared& obj) {
    if (!obj.keys->empty()) {
      obj.mouse = mouse;
    }
  });
}

void Joystick::receiveKeyPressed(Qt::Key key) {
  shared->keys->insert(key);
}

void Joystick::receiveKeyReleased(Qt::Key key) {
  shared->keys->remove(key);
}

static_block {
  Factory::processing.insert<Joystick>();
};
