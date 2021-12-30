//
// Created by jose-cruz on 17/07/2021.
//

#ifndef PROJECT_UNIFICATION_GRSIMCOMMUTILS_H
#define PROJECT_UNIFICATION_GRSIMCOMMUTILS_H

#include <soccer-common/soccer-common.h>
#include <protobufs/protobufs.h>
#include "Entities/Entities.h"
#include "Packages/Packages.h"

namespace Simulation::GrSim {
  namespace Replacement {
    /*!
     * @brief receives position and velocity in millimeters and sends in meters. Receives angle in
     * radians and sends in radians.
     */
    inline RoboCupSSL::SimulatorCommand robot(bool isYellow,
                                              int id,
                                              float angle,
                                              QPointF position,
                                              QPointF velocity,
                                              float angularVelocity = 0,
                                              bool isOnField = true) {
      RoboCupSSL::TeleportRobot robot;
      {
        RoboCupSSL::RobotId robotId;
        robotId.set_team(isYellow ? RoboCupSSL::Team::YELLOW : RoboCupSSL::Team::BLUE);
        robotId.set_id(id);
        robot.set_allocated_id(new RoboCupSSL::RobotId(robotId));
      }
      robot.set_orientation(angle);
      {
        position /= 1000.0;
        robot.set_x(static_cast<float>(position.x()));
        robot.set_y(static_cast<float>(position.y()));
      }
      {
        velocity /= 1000.0;
        robot.set_v_x(static_cast<float>(velocity.x()));
        robot.set_v_y(static_cast<float>(velocity.y()));
      }
      robot.set_v_angular(angularVelocity);
      robot.set_present(isOnField);

      RoboCupSSL::SimulatorControl control;
      control.add_teleport_robot()->CopyFrom(robot);
      RoboCupSSL::SimulatorCommand command;
      command.set_allocated_control(new RoboCupSSL::SimulatorControl(control));
      return command;
    }

    /*!
     * @brief receives in millimeters and sends in meters.
     */
    inline RoboCupSSL::SimulatorCommand ball(QVector3D position,
                                             QVector3D velocity,
                                             bool teleportSafely = false,
                                             bool isRolling = false) {
      RoboCupSSL::TeleportBall ball;
      {
        position /= 1000.0;
        ball.set_x(position.x());
        ball.set_y(position.y());
        ball.set_z(position.z());
      }

      {
        velocity /= 1000.0;
        ball.set_vx(velocity.x());
        ball.set_vy(velocity.y());
        ball.set_vz(velocity.z());
      }

      ball.set_teleport_safely(teleportSafely);
      ball.set_roll(isRolling);

      RoboCupSSL::SimulatorControl control;
      control.set_allocated_teleport_ball(new RoboCupSSL::TeleportBall(ball));
      RoboCupSSL::SimulatorCommand command;
      command.set_allocated_control(new RoboCupSSL::SimulatorControl(control));
      return command;
    }

    /*!
     * @brief receives in millimeters and sends in meters.
     */
    inline RoboCupSSL::SimulatorCommand ball(const QPointF& position,
                                             const QPointF& velocity,
                                             bool teleportSafely = false,
                                             bool isRolling = false) {
      return ball(QVector3D(position), QVector3D(velocity), teleportSafely, isRolling);
    }
  } // namespace Replacement

  inline RoboCupSSL::RobotControl toRobotControl(const QVector<SSLCommand>& packets,
                                                 const Robots<Robot>& allies) {
    RoboCupSSL::RobotControl control;
    for (const auto& packet : packets) {
      if (auto robotIt = allies.findById(packet.id()); robotIt != allies.end()) {
        if (auto cmd = control.add_robot_commands()) {
          cmd->set_id(packet.id());

          if (auto move = cmd->mutable_move_command()->mutable_local_velocity()) {
            move->set_angular(static_cast<float>(packet.angularVelocity()));
            Point fixedVelocity = packet.velocity().rotatedCW(robotIt->angle());
            move->set_forward(static_cast<float>(fixedVelocity.x()));
            move->set_left(static_cast<float>(fixedVelocity.y()));
          } else {
            throw std::runtime_error("move command for robot is nullptr.");
          }
          cmd->set_kick_speed(
              (packet.front() || packet.chip()) ? static_cast<float>(packet.kickSpeed()) : 0.0f);
          cmd->set_kick_angle(packet.chip() ? 45.0 : 0.0);
          cmd->set_dribbler_speed(packet.dribbler() ? 6000.0 : 0.0);
        } else {
          throw std::runtime_error("command for robot is nullptr.");
        }
      }
    }
    return control;
  }
} // namespace Simulation::GrSim

#endif // PROJECT_UNIFICATION_GRSIMCOMMUTILS_H
