//
// Created by jose-cruz on 09/02/2021.
//

#include "DrawGameContext.h"

void DrawGameContext::update(const Ball& t_ball,
                             const QVector<Robot>& t_blueRobots,
                             const QVector<Robot>& t_yellowRobots) {
  ball = t_ball;
  blueRobots = t_blueRobots;
  yellowRobots = t_yellowRobots;
}

void DrawGameContext::run(GameVisualizerPainter2D* f) {
  for (const auto& robot : blueRobots) {
    drawRobot(f, robot, DarkBlue, Blue);
  }
  for (const auto& robot : yellowRobots) {
    drawRobot(f, robot, DarkYellow, Yellow);
  }

  f->drawFilledCircle(ball, 2.1, DarkOrange);
  f->drawFilledCircle(ball, 1.5, Orange);
}

std::unique_ptr<Painting> DrawGameContext::clone() const {
  return std::make_unique<DrawGameContext>(*this);
}
