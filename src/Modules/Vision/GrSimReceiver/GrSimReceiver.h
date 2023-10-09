//
// Created by jose-cruz on 05/02/2021.
//

#ifndef PROJECT_UNIFICATION_GRSIMRECEIVER_H
#define PROJECT_UNIFICATION_GRSIMRECEIVER_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include <protobufs/protobufs.h>
#include <deque>

#include "Modules/Modules.h"
#include "Modules/Vision/VisionUtils/VisionUtils.h"

class GrSimReceiver : public Vision {
 public:
  explicit GrSimReceiver(QThreadPool* threadPool);

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  struct Args {
    Parameters::Arg<QString> ip = "224.5.23.2";
    Parameters::Arg<int> port = 10020;
    Parameters::Arg<QString> inet = QNetworkInterface::allInterfaces().front().name();
    Parameters::Arg<bool> isYellow = true;
    Parameters::Arg<bool> isAttackingToRight = true;
  };
  Args args;

  struct Shared {
    SharedValue<std::deque<RoboCupSSL::SSL_WrapperPacket>> packets;
  };
  SharedWrapper<Shared, std::mutex> shared;

  struct Cte {
    struct DivB {
      static constexpr double FIELD_LENGTH = 9000;
      static constexpr double FIELD_WIDTH = 6000;
      static constexpr double GOAL_DEPTH = 180;
      static constexpr double GOAL_WIDTH = 1000;
      static constexpr double PENALTY_AREA_DEPTH = 1000;
      static constexpr double PENALTY_AREA_WIDTH = 2000;
      static constexpr double BOUNDARY_WIDTH = 300;
      static constexpr double GOAL_CENTER_TO_PENALTY_MARK = 6000;
    };
  };

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socket;
  std::deque<RoboCupSSL::SSL_WrapperPacket> packets;

  GameVisualizerBase::Key fieldKey;
  GameVisualizerBase::Key detectionKey;
  DrawSSLClientDetection detectionDraw;

  LossDetection loss;

  void setupClient();

 private slots:
  void receiveDatagrams();
};

#endif // PROJECT_UNIFICATION_GRSIMRECEIVER_H
