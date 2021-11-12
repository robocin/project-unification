//
// Created by jose-cruz on 09/02/2021.
//

#include "DrawField.h"
#include "Modules/Vision/VisionUtils/DrawCIninho/DrawCIninho.h"

DrawField::DrawField(const Field& field, bool drawFieldMarkings, bool drawCIninho) :
    field(field),
    drawFieldMarkings(drawFieldMarkings),
    drawCIninho(drawCIninho) {
}
void DrawField::run(GameVisualizerPainter2D* f) {
  QPolygonF fieldLines({QPointF(-field.length() / 2, -field.width() / 2),
                        QPointF(-field.length() / 2, field.width() / 2),
                        QPointF(field.length() / 2, field.width() / 2),
                        QPointF(field.length() / 2, -field.width() / 2)});
  /* field border and middle field. */ {
    f->drawPolygon(fieldLines, Color::White, 0.5);
    f->drawLine(QPointF(0, field.width() / 2), QPointF(0, -field.width() / 2), Color::White, 0.5);
  }
  /* field corners. */ {
    Extends<QPointF> dir(0, 1);
    for (const auto& pt : fieldLines) {
      f->drawFilledPolygon(
          QPolygonF{pt, pt + dir * (8.5 + 0.25), pt + dir.rotatedCW90() * (8.5 + 0.25)},
          Color::White);
      dir.rotateCW90();
    }
  }

  QPolygonF goalLines({QPointF(-field.goalDepth() / 2, -field.goalWidth() / 2),
                       QPointF(-field.goalDepth() / 2, field.goalWidth() / 2),
                       QPointF(field.goalDepth() / 2, field.goalWidth() / 2),
                       QPointF(field.goalDepth() / 2, -field.goalWidth() / 2)});
  /* field goal (left and right) */ {
    f->drawPolygon(goalLines.translated(-field.length() / 2 - field.goalDepth() / 2, 0),
                   Color::White,
                   0.5);
    f->drawPolygon(goalLines.translated(field.length() / 2 + field.goalDepth() / 2, 0),
                   Color::White,
                   0.5);
  }

  QPolygonF penaltyAreaLines(
      {QPointF(-field.penaltyAreaDepth() / 2, -field.penaltyAreaWidth() / 2),
       QPointF(-field.penaltyAreaDepth() / 2, field.penaltyAreaWidth() / 2),
       QPointF(field.penaltyAreaDepth() / 2, field.penaltyAreaWidth() / 2),
       QPointF(field.penaltyAreaDepth() / 2, -field.penaltyAreaWidth() / 2)});
  /* field goal area (left and right) */ {
    f->drawPolygon(
        penaltyAreaLines.translated(-field.length() / 2 + field.penaltyAreaDepth() / 2, 0),
        Color::White,
        0.5);
    f->drawPolygon(
        penaltyAreaLines.translated(field.length() / 2 - field.penaltyAreaDepth() / 2, 0),
        Color::White,
        0.5);
  }

  if (drawCIninho) {
    DrawCIninho(QPointF(0, 0), 10).run(f);
  }
  if (drawFieldMarkings) {
    auto draw = [f](const QString& text, const Point& pos, const Point& textOffset = Point(0, 3)) {
      f->drawFilledCircle(pos, 1, Color::Magenta);
      f->drawText(text, pos + textOffset, 3, Color::Red);
    };

    draw("max", field.max());
    draw("center", Field::center());
    draw("min", field.min(), Point(0, -3));

    draw("AGOTop", field.allyGoalOutsideTop());
    draw("AGOCenter", field.allyGoalOutsideCenter());
    draw("AGOBottom", field.allyGoalOutsideBottom());

    draw("AGITop", field.allyGoalInsideTop());
    draw("AGICenter", field.allyGoalInsideCenter(), Point(0, -3));
    draw("AGIBottom", field.allyGoalInsideBottom(), Point(0, -3));

    draw("APACTop", field.allyPenaltyAreaCornerTop());
    draw("APACBottom", field.allyPenaltyAreaCornerBottom(), Point(0, -3));

    draw("EGOTop", field.enemyGoalOutsideTop());
    draw("EGOCenter", field.enemyGoalOutsideCenter());
    draw("EGOBottom", field.enemyGoalOutsideBottom());

    draw("EGITop", field.enemyGoalInsideTop());
    draw("EGICenter", field.enemyGoalInsideCenter(), Point(0, -3));
    draw("EGIBottom", field.enemyGoalInsideBottom(), Point(0, -3));

    draw("EPACTop", field.enemyPenaltyAreaCornerTop());
    draw("EPACBottom", field.enemyPenaltyAreaCornerBottom(), Point(0, -3));
  }
}
std::unique_ptr<Painting> DrawField::clone() const {
  return std::make_unique<DrawField>(*this);
}
