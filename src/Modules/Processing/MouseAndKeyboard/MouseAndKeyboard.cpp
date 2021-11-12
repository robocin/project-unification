//
// Created by jose-cruz on 06/08/2021.
//

#include "MouseAndKeyboard.h"

MouseAndKeyboard::MouseAndKeyboard(int index, QThreadPool* threadPool) :
    Processing(index, threadPool) {
}

void MouseAndKeyboard::buildParameters() {
  parameters()["Position Key"] =
      Parameters::ComboBox(args.targetKey, {Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_T});
}

void MouseAndKeyboard::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &Vision::sendFrame,
          this,
          &MouseAndKeyboard::receiveFrame,
          Qt::DirectConnection);

  connect(modules->vision(),
          &Vision::sendField,
          this,
          &MouseAndKeyboard::receiveField,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::relativeMousePos,
          this,
          &MouseAndKeyboard::receiveMousePos,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::onKeyPress,
          this,
          &MouseAndKeyboard::receiveKeyPressed,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::onKeyRelease,
          this,
          &MouseAndKeyboard::receiveKeyReleased,
          Qt::DirectConnection);
}

void MouseAndKeyboard::init(const Modules* modules) {
  targetKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Top);
}

void MouseAndKeyboard::update() {
  shared->field.extract_to(field);
  if (auto f = shared->frame.get_optional_and_reset()) {
    if (auto it = f->allies().findById(index()); it != f->allies().end()) {
      robot = *it;
    }
    frame.emplace(*f);
  }
  shared->mouse.extract_to(mouse);
}

void MouseAndKeyboard::exec() {
  if (!field || !frame || !robot || !mouse) {
    return;
  }

  targetKey.draw([mouse = this->mouse](GameVisualizerPainter2D* f) {
    f->drawFilledCircle(mouse.value(), 45, Color::Red);
  });

  // TODO: emit sendCommand();
}

void MouseAndKeyboard::receiveField(const Field& field) {
  shared->field = field;
}

void MouseAndKeyboard::receiveFrame(const Frame& frame) {
  shared->frame = frame;
  runInParallel();
}

void MouseAndKeyboard::receiveMousePos(const Point& mouse) {
  shared.apply([&](Shared& obj) {
    if (obj.keys->contains(args.targetKey)) {
      obj.mouse = mouse;
    }
  });
}

void MouseAndKeyboard::receiveKeyPressed(Qt::Key key) {
  shared->keys->insert(key);
}

void MouseAndKeyboard::receiveKeyReleased(Qt::Key key) {
  shared->keys->remove(key);
}

static_block {
  Factory::processing.insert<MouseAndKeyboard>();
};
