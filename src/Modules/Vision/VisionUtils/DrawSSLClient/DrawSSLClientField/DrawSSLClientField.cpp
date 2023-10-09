#include "DrawSSLClientField.h"
#include <protobufs/protobufs.h>

#include "Modules/Vision/VisionUtils/DrawCIninho/DrawCIninho.h"

DrawSSLClientField::DrawSSLClientField(const RoboCupSSL::SSL_GeometryFieldSize& field,
                                       bool drawCIninho,
                                       const QColor& color) :
    drawCIninho(drawCIninho),
    color(color) {
  for (const auto& line : field.field_lines()) {
    lines +=
        Line{QLineF(line.p1().x(), line.p1().y(), line.p2().x(), line.p2().y()), line.thickness()};
  }

  for (const auto& arc : field.field_arcs()) {
    arcs += Arc{QPointF(arc.center().x(), arc.center().y()),
                arc.radius() - arc.thickness() / 2.0,
                arc.radius() + arc.thickness() / 2.0,
                arc.a1(),
                arc.a2()};
  }
}

void DrawSSLClientField::run(Painter2DBase* f) {
  for (const auto& line : lines) {
    f->drawLine(line.line.p1(), line.line.p2(), color, line.thickness);
  }

  for (const auto& arc : arcs) {
    // f->drawArc(arc.origin, arc.innerRadius, arc.outterRadius, arc.theta1, arc.theta2, color);
    if (arc.origin.isNull() && drawCIninho) {
      qreal scale = 1.2 * arc.outterRadius * std::sin(PI / 4);
      DrawCIninho draw(arc.origin, scale);
      draw.run(f);
    }
  }
}

std::unique_ptr<Painting> DrawSSLClientField::clone() const {
  return std::make_unique<DrawSSLClientField>(*this);
}
