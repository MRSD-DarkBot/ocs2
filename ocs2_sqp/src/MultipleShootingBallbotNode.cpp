//
// Created by rgrandia on 09.11.20.
//

#include <ros/init.h>

#include <ocs2_ros_interfaces/mpc/MPC_ROS_Interface.h>
#include "ocs2_ballbot_example/BallbotInterface.h"
#include "ocs2_sqp/MultipleShootingMpc.h"
#include "ocs2_sqp/MultipleShootingSolver.h"
#include "ocs2_mpc/MPC_Settings.h"

int main(int argc, char **argv)
{
  const std::string robotName = "ballbot";

  // task file
  std::vector<std::string> programArgs{};
  ::ros::removeROSArgs(argc, argv, programArgs);
  if (programArgs.size() <= 1)
  {
    throw std::runtime_error("No task file specified. Aborting.");
  }
  std::string taskFileFolderName = std::string(programArgs[1]);

  // Initialize ros node
  ros::init(argc, argv, robotName + "_mpc");
  ros::NodeHandle nodeHandle;

  // Robot interface
  ocs2::ballbot::BallbotInterface ballbotInterface(taskFileFolderName);

  // Set this one up.
  ocs2::MultipleShootingSolverSettings settings;
  settings.N = 10;
  settings.nx = 10;
  settings.nu = 3;
  settings.sqp_iteration = 10;

  ocs2::mpc::Settings mpcSettings = ballbotInterface.mpcSettings();
  ocs2::MultipleShootingMpc mpc(mpcSettings, settings, &ballbotInterface.getDynamics(), &ballbotInterface.getCost());

  ocs2::MPC_ROS_Interface mpcNode(mpc, robotName);
  mpcNode.launchNodes(nodeHandle);

  // Successful exit
  return 0;
}