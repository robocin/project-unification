//
// Created by jose-cruz on 17/07/2021.
//

#ifndef PROJECT_UNIFICATION_GRSIMCOMM_H
#define PROJECT_UNIFICATION_GRSIMCOMM_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include "Modules/Modules.h"
#include "GrSimCommUtils/GrSimCommUtils.h"

class GrSimComm : public Acting {
  Q_OBJECT
 public:
  explicit GrSimComm(QThreadPool* threadPool);

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  class Args {
    struct SocketArgs {
      Parameters::Arg<QString> ip;
      Parameters::Arg<int> port;
    };

   public:
    SocketArgs simulation{"127.0.0.1", 10302};
    SocketArgs replacement{"127.0.0.1", 10300};

    Parameters::Arg<bool> isReplacementEnabled = false;
    Parameters::Arg<float> replacementAngle = 0;
  };
  Args args;

  struct Shared {
    SharedOptional<Frame> frame;
    SharedOptional<Point> replacementPos;
    SharedValue<QSet<Qt::Key>> pressedKeys;
    SharedValue<QVector<SSLCommand>> packets;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socket;
  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> replacementSocket;

  QVector<SSLCommand> packets;
  std::optional<Frame> frame;

  void setupClient();
  void setupReplacementClient();

 private slots:
  void receiveRelativeMousePos(const Point& position);
  void receiveKeyPress(Qt::Key key);
  void receiveKeyRelease(Qt::Key key);
  void receiveCommand(const Command& command);
  void receiveFrame(const Frame& frame);

 private:
  static std::optional<Point> isBallReplacement(const Shared& obj);
  static std::optional<std::tuple<bool, int, Point>> isRobotReplacement(const Shared& obj);

 signals:
  void sendReplacementDatagram(const QByteArray& datagram);
};

#endif // PROJECT_UNIFICATION_GRSIMCOMM_H
