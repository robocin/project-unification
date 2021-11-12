//
// Created by jose-cruz on 09/02/2021.
//

#ifndef PROJECT_UNIFICATION_FIRASIMCOMM_H
#define PROJECT_UNIFICATION_FIRASIMCOMM_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include "Modules/Modules.h"
#include "Packages/Packages.h"
#include "FIRASimCommUtils/FIRASimCommUtils.h"

class FIRASimComm : public Acting {
 public:
  explicit FIRASimComm(QThreadPool* threadPool);

 protected:
  void buildParameters() override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

  struct Args {
    Parameters::Arg<QString> ip = "127.0.0.1";
    Parameters::Arg<int> port = 20011;

    Parameters::Arg<bool> isReplacementEnabled = false;
    Parameters::Arg<float> replacementAngle = 0;
  };
  Args args;

  struct Shared {
    SharedOptional<Field> field;
    SharedOptional<Frame> frame;
    SharedOptional<bool> hasIsYellow;
    SharedValue<QVector<VSSCommand>> commands;

    SharedOptional<Point> replacementPos;
    SharedValue<QSet<Qt::Key>> pressedKeys;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socket;

  std::optional<Field> field;
  std::optional<Frame> frame;
  std::optional<bool> hasIsYellow;
  QVector<VSSCommand> commands;

  void setupClient();

 private slots:
  void receiveIsYellow(bool value);
  void receiveField(const Field& package);
  void receiveFrame(const Frame& package);
  void receiveCommand(const Command& command);

  void receiveRelativeMousePos(const Point& position);
  void receiveKeyPress(Qt::Key key);
  void receiveKeyRelease(Qt::Key key);

 private:
  static std::optional<Point> isBallReplacement(const Shared& obj);
  static std::optional<std::tuple<bool, int, Point>> isRobotReplacement(const Shared& obj);
};

#endif // PROJECT_UNIFICATION_FIRASIMCOMM_H
