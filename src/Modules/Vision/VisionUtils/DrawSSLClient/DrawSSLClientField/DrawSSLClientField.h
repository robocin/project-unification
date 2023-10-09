#ifndef PROJECT_UNIFICATION_DRAWSSLCLIENTFIELD_H
#define PROJECT_UNIFICATION_DRAWSSLCLIENTFIELD_H

#include <soccer-common/soccer-common.h>

namespace RoboCupSSL {
  class SSL_GeometryFieldSize;
}

class DrawSSLClientField : public Painting {
  struct Line {
    QLineF line;
    qreal thickness;
  };

  struct Arc {
    QPointF origin;
    qreal innerRadius;
    qreal outterRadius;
    qreal theta1;
    qreal theta2;
  };

  QVector<Line> lines;
  QVector<Arc> arcs;

  bool drawCIninho;
  QColor color;

 public:
  DrawSSLClientField(const RoboCupSSL::SSL_GeometryFieldSize& field,
                     bool drawCIninho = false,
                     const QColor& color = Color::White);

  void run(Painter2DBase* f) override;
  std::unique_ptr<Painting> clone() const override;
};

#endif // PROJECT_UNIFICATION_DRAWSSLCLIENTFIELD_H
