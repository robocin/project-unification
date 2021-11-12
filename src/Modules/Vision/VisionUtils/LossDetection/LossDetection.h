//
// Created by jose-cruz on 13/06/2021.
//

#ifndef PROJECT_UNIFICATION_LOSSDETECTION_H
#define PROJECT_UNIFICATION_LOSSDETECTION_H

#include <QMap>
#include <protobufs/protobufs.h>
#include <optional>

class LossDetection {
  using DetectionFrame = RoboCupSSL::SSL_DetectionFrame;

  QMap<int, std::optional<int>> frameNumber;

 public:
  inline LossDetection() = default;

  enum class Type { Swapped, Lost };

  inline std::optional<Type> operator()(const DetectionFrame& detection) {
    int delta = 0;
    int id = static_cast<int>(detection.camera_id());
    if (frameNumber[id]) {
      delta = static_cast<int>(detection.frame_number()) - frameNumber[id].value();
    }
    frameNumber[id] = detection.frame_number();

    if (delta < 0) {
      return Type::Swapped;
    }
    if (delta > 1) {
      return Type::Lost;
    }
    return std::nullopt;
  }

  inline void clear() {
    for (std::optional<int>& value : frameNumber) {
      value.reset();
    }
  }
};

#endif // PROJECT_UNIFICATION_LOSSDETECTION_H
