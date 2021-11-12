#ifndef PROJECT_UNIFICATION_DRAWSSLCLIENTDETECTION_H
#define PROJECT_UNIFICATION_DRAWSSLCLIENTDETECTION_H

#include <soccer-common/soccer-common.h>

namespace RoboCupSSL {
  class SSL_DetectionFrame;
}

class DrawSSLClientDetection : public Painting {
  inline static const auto DarkBlue = QColor::fromRgbF(0.0706, 0.2314, 0.6275);
  inline static const auto DarkYellow = QColor::fromRgbF(0.8, 0.6157, 0.0);
  inline static const auto DarkOrange = QColor::fromRgbF(0.8706, 0.3490, 0.0);

  inline static const auto Blue = QColor::fromRgbF(0.2549, 0.4941, 1.0);
  inline static const auto Yellow = QColor::fromRgbF(1.0, 0.9529, 0.2431);
  inline static const auto Orange = QColor::fromRgbF(1.0, 0.5059, 0.0);

  enum class RobotColor { Blue, Yellow };

  struct Robot {
    QPointF position;
    std::optional<qreal> angle;
    std::optional<int> id;
    qreal confidence;
    RobotColor color;
  };

  using Ball = QPointF;

  QHash<int, QVector<Robot>> cameraRobots;
  QHash<int, QVector<Ball>> cameraBalls;

 public:
  void update(const RoboCupSSL::SSL_DetectionFrame& detection);

  void run(GameVisualizerPainter2D* f) override;
  std::unique_ptr<Painting> clone() const override;
};

#endif // PROJECT_UNIFICATION_DRAWSSLCLIENTDETECTION_H
