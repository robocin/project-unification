#include "DrawSSLClientDetection.h"
#include <protobufs/protobufs.h>

void DrawSSLClientDetection::update(const RoboCupSSL::SSL_DetectionFrame& detection) {
  int cameraId = static_cast<int>(detection.camera_id());

  /* robots */ {
    auto& robots = cameraRobots[cameraId];
    robots.clear();

    QList colorAndRobots = {std::tuple(RobotColor::Blue, detection.robots_blue()),
                            std::tuple(RobotColor::Yellow, detection.robots_yellow())};

    for (const auto& [color, detectionRobots] : colorAndRobots) {
      for (const auto& robot : detectionRobots) {
        QPointF position(robot.x(), robot.y());
        std::optional<qreal> angle(
            robot.has_orientation() ? std::make_optional(robot.orientation()) : std::nullopt);
        std::optional<int> id(robot.has_robot_id() ? std::make_optional(robot.robot_id()) :
                                                     std::nullopt);
        qreal confidence = robot.confidence();
        robots += Robot{position, angle, id, confidence, color};
      }
    }
  }

  /* balls */ {
    auto& balls = cameraBalls[cameraId];
    balls.clear();
    for (const auto& ball : detection.balls()) {
      balls += Ball{QPointF(ball.x(), ball.y())};
    }
  }
}

void DrawSSLClientDetection::run(Painter2DBase* f) {
  for (auto& robots : cameraRobots) {
    for (auto& robot : robots) {
      /* confidence bar border. */ {
        QColor color = robot.color == RobotColor::Blue ? DarkBlue : DarkYellow;
        QPolygonF polygon;
        polygon += QPointF(-93, 163);
        polygon += QPointF(+93, 163);
        polygon += QPointF(+93, 127);
        polygon += QPointF(-93, 127);
        polygon.translate(robot.position);
        f->drawPolygon(polygon, color, 6);
      }

      /* confidence bar. */ {
        QColor color = robot.color == RobotColor::Blue ? Blue : Yellow;
        f->drawFilledRectangle(robot.position + QPointF(-90, 130),
                               robot.position + QPointF(-90.0 + 180.0 * robot.confidence, 160),
                               color);
      }

      /* robot body border. */ {
        QColor color = robot.color == RobotColor::Blue ? DarkBlue : DarkYellow;
        if (!robot.angle) {
          f->drawFilledCircle(robot.position, 90, color);
        } else {
          qreal theta1 = Geometry::degreesToRadians(40) + *robot.angle;
          qreal theta2 = (2.0 * PI - Geometry::degreesToRadians(40)) + *robot.angle;
          // f->drawCircleSegment(robot.position, 90, theta1, theta2, color);
        }
      }

      /* robot body. */ {
        QColor color = robot.color == RobotColor::Blue ? Blue : Yellow;
        if (!robot.angle) {
          f->drawFilledCircle(robot.position, 80, color);
        } else {
          qreal theta1 = Geometry::degreesToRadians(40) + *robot.angle;
          qreal theta2 = (2.0 * PI - Geometry::degreesToRadians(40)) + *robot.angle;
          // f->drawCircleSegment(robot.position, 80, theta1, theta2, color);
        }
      }

      /* id. */ {
        f->drawText(robot.id ? QString::number(*robot.id) : QString("?"),
                    robot.position,
                    120,
                    Color::Black);
      }
    }
  }

  for (auto& balls : cameraBalls) {
    for (auto& ball : balls) {
      f->drawFilledCircle(ball, 21, DarkOrange);
      f->drawFilledCircle(ball, 15, Orange);
    }
  }
}

std::unique_ptr<Painting> DrawSSLClientDetection::clone() const {
  return std::make_unique<DrawSSLClientDetection>(*this);
}
