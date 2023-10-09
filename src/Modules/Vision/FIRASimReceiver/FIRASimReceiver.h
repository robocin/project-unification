//
// Created by jose-cruz on 08/02/2021.
//

#ifndef PROJECT_UNIFICATION_FIRASIMRECEIVER_H
#define PROJECT_UNIFICATION_FIRASIMRECEIVER_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include <protobufs/protobufs.h>
#include <deque>

#include "Modules/Modules.h"
#include "Modules/Vision/VisionUtils/VisionUtils.h"

class FIRASimReceiver : public Vision {
 public:
  explicit FIRASimReceiver(QThreadPool* threadPool);

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

    Parameters::Arg<bool> drawFieldMarkings = false;
    Parameters::Arg<bool> drawCIninho = true;
  };
  Args args;

  struct Shared {
    SharedValue<std::deque<fira_message::sim_to_ref::Environment>> packets;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socket;

  std::deque<fira_message::sim_to_ref::Environment> packets;

  GameVisualizerBase::Key detectionKey;
  GameVisualizerBase::Key fieldKey;
  DrawGameContext drawGameContext;

  std::optional<Field> field;

  void setupClient();

 private slots:
  void receiveDatagram();
};

#endif // PROJECT_UNIFICATION_FIRASIMRECEIVER_H
