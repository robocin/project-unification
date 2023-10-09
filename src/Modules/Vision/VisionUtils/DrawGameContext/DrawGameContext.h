//
// Created by jose-cruz on 09/02/2021.
//

#ifndef VSS_UNIFICATION_DRAWGAMECONTEXT_H
#define VSS_UNIFICATION_DRAWGAMECONTEXT_H

#include <soccer-common/soccer-common.h>
#include "Entities/Entities.h"

class DrawGameContext : public Painting {
  inline static const auto DarkBlue = QColor::fromRgbF(0.0706, 0.2314, 0.6275);
  inline static const auto DarkYellow = QColor::fromRgbF(0.8, 0.6157, 0.0);
  inline static const auto DarkOrange = QColor::fromRgbF(0.8706, 0.3490, 0.0);

  inline static const auto Blue = QColor::fromRgbF(0.2549, 0.4941, 1.0);
  inline static const auto Yellow = QColor::fromRgbF(1.0, 0.9529, 0.2431);
  inline static const auto Orange = QColor::fromRgbF(1.0, 0.5059, 0.0);

  static constexpr float AXIS = 7.5;

  QPointF ball;
  QVector<Robot> blueRobots;
  QVector<Robot> yellowRobots;

  inline static void drawRobot(Painter2DBase* f,
                               const Robot& robot,
                               const QColor& bodyColor,
                               const QColor& primaryColor) {
    QPolygonF body(QRectF(-AXIS / 2, -AXIS / 2, AXIS, AXIS));
    for (auto& pt : body) {
      pt = Geometry2D::rotateCCW(pt, robot.angle());
    }
    float eps = AXIS / 11;
    QPolygonF primaryTag(QRectF(-AXIS / 2 + eps, -AXIS / 2 + eps, 4 * eps, 9 * eps));
    for (auto& pt : primaryTag) {
      pt = Geometry2D::rotateCCW(pt, robot.angle());
    }

    QPolygonF secondaryTag(QRectF(-AXIS / 2 + 6 * eps, -AXIS / 2 + eps, 4 * eps, 9 * eps));
    for (auto& pt : secondaryTag) {
      pt = Geometry2D::rotateCCW(pt, robot.angle());
    }

    f->drawFilledPolygon(body.translated(robot.position()), Color::Black);
    f->drawFilledPolygon(primaryTag.translated(robot.position()), primaryColor);
    {
      QColor color = Color::Black;
      static constexpr std::array colorById{Color::Red,
                                            Color::Green,
                                            Color::Magenta,
                                            Color::LightBlue,
                                            Color::Purple};
      if (robot.id() < colorById.size()) {
        color = colorById[robot.id()];
      }
      f->drawFilledPolygon(secondaryTag.translated(robot.position()), color);
    }
    f->drawFilledCircle(robot.position(), AXIS / 4, Color::Black);
    f->drawText(QString::number(robot.id()), robot.position(), AXIS / 2, Color::White);
  }

 public:
  void update(const Ball& t_ball,
              const QVector<Robot>& t_blueRobots,
              const QVector<Robot>& t_yellowRobots);

  void run(Painter2DBase* f) override;
  std::unique_ptr<Painting> clone() const override;
};

#endif // VSS_UNIFICATION_DRAWGAMECONTEXT_H
