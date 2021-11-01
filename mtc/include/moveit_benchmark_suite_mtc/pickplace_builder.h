/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2021, Captain Yoshi
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Captain Yoshi
   Desc:   Query builder for the pick and place benchmark.
*/

#pragma once

// ROS
#include <ros/ros.h>

// MoveIt
#include <moveit/planning_scene/planning_scene.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

// MTC
#include <moveit/task_constructor/task.h>
#include <moveit/task_constructor/stages/compute_ik.h>
#include <moveit/task_constructor/stages/connect.h>
#include <moveit/task_constructor/stages/current_state.h>
#include <moveit/task_constructor/stages/generate_grasp_pose.h>
#include <moveit/task_constructor/stages/generate_pose.h>
#include <moveit/task_constructor/stages/generate_place_pose.h>
#include <moveit/task_constructor/stages/modify_planning_scene.h>
#include <moveit/task_constructor/stages/move_relative.h>
#include <moveit/task_constructor/stages/move_to.h>
#include <moveit/task_constructor/stages/predicate_filter.h>
#include <moveit/task_constructor/solvers/joint_interpolation.h>
#include <moveit/task_constructor/solvers/cartesian_path.h>
#include <moveit/task_constructor/solvers/pipeline_planner.h>
#include <moveit_task_constructor_msgs/ExecuteTaskSolutionAction.h>

#include <moveit_benchmark_suite/dataset.h>
#include <moveit_benchmark_suite_mtc/pickplace.h>

namespace moveit_benchmark_suite_mtc
{
struct Stage
{
  std::string solver;
  std::string path_constraints;
  double timeout;
};

struct Task
{
  std::string name;
  std::string global_solver;
  std::string global_path_constraints;
  double global_timeout = 5.0;

  std::map<std::string, Stage> stage_map;
};

class PickPlaceConfig
{
public:
  PickPlaceConfig();
  PickPlaceConfig(const std::string& ros_namespace);
  virtual ~PickPlaceConfig() = default;

  /** \brief Set the ROS namespace the node handle should use for parameter lookup */
  void setNamespace(const std::string& ros_namespace);

  const PickPlaceParameters& getParameters() const;
  const std::map<std::string, std::string>& getScenes() const;
  const std::map<std::string, solvers::PlannerInterfacePtr>& getSolvers() const;
  const std::map<std::string, moveit_msgs::Constraints>& getConstraints() const;
  const std::map<std::string, Task>& getTasks() const;

  static SolverType resolveSolverType(const std::string& type);

protected:
  void readBenchmarkConfig(const std::string& ros_namespace);

  void readParameters(ros::NodeHandle& nh);
  void readSolvers(ros::NodeHandle& nh);
  void readConstraints(ros::NodeHandle& nh);
  void readTasks(ros::NodeHandle& nh);
  void readScenes(ros::NodeHandle& nh);

  void fillTaskStages(Task& task, const XmlRpc::XmlRpcValue& node);

  PickPlaceParameters parameters_;
  std::map<std::string, std::string> scene_map_;  // name, filepath
  std::map<std::string, solvers::PlannerInterfacePtr> solver_map_;
  std::map<std::string, moveit_msgs::Constraints> constraints_map_;
  std::map<std::string, Task> task_map_;
};

using namespace moveit_benchmark_suite;

class PickPlaceBuilder
{
public:
  void buildQueries();

  const PickPlaceConfig& getConfig() const;
  const QuerySetup& getQuerySetup() const;
  const std::vector<PickPlaceQueryPtr>& getQueries() const;

protected:
  void buildScenes();
  void buildTasks();

  QuerySetup query_setup_;
  PickPlaceConfig config_;

  std::vector<PickPlaceQueryPtr> queries_;
  std::vector<moveit_msgs::PlanningScene> scenes_;
  std::vector<TaskProperty> tasks_;
};

}  // namespace moveit_benchmark_suite_mtc
