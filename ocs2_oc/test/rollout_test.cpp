/******************************************************************************
Copyright (c) 2017, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory>

#include <gtest/gtest.h>

#include <ocs2_core/dynamics/LinearSystemDynamics.h>
#include <ocs2_core/control/LinearController.h>

#include "ocs2_oc/rollout/TimeTriggeredRollout.h"
#include "ocs2_oc/rollout/NEWTimeTriggeredRollout.h"
#include "ocs2_oc/test/EXP1.h"

using namespace ocs2;

enum {
  STATE_DIM = 2,
  INPUT_DIM = 1
};

class TestLogicRules : public HybridLogicRules
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	TestLogicRules() = default;
	virtual ~TestLogicRules() = default;
	void set(const scalar_array_t& eventTimes) { eventTimes_ = eventTimes; }
	void update() override {}
	void rewind(const scalar_t& lowerBoundTime, const scalar_t& upperBoundTime) override {}
 protected:
   void insertModeSequenceTemplate(const logic_template_type& modeSequenceTemplate,
		  const scalar_t& initTime, const scalar_t& finalTime) override {};

};

TEST(time_triggered_rollout_test, time_triggered_rollout_test) {

	double initTime = 0.0;
	double finalTime = 10.0;

	Eigen::Matrix2d A;
	A << -2, -1, 1, 0;
	Eigen::Vector2d B;
	B << 1, 0;

	typedef LinearSystemDynamics<STATE_DIM, INPUT_DIM> SecondOrderSystem;
	SecondOrderSystem systemDynamics(A, B);

	// controller
	SecondOrderSystem::scalar_array_t cntTimeStamp {initTime, finalTime};
	SecondOrderSystem::input_vector_array_t uff(2, SecondOrderSystem::input_vector_t::Ones());
	SecondOrderSystem::input_state_matrix_array_t k(2, SecondOrderSystem::input_state_matrix_t::Zero());
    using controller_t = ocs2::LinearController<STATE_DIM, INPUT_DIM>;
    auto controller = std::unique_ptr<controller_t>(new controller_t(cntTimeStamp, uff, k));

    SecondOrderSystem::state_vector_t initState;
    initState.setZero();

	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/
	// partitioning times
	std::vector<double> partitioningTimes{0.0, 4.0, 5.0, 7.0};

	// event times
	std::shared_ptr<TestLogicRules> logicRules(new TestLogicRules());
	HybridLogicRulesMachine logicRulesMachine(logicRules);

	// No switch
	std::vector<double> logicRulesEventTimes = std::vector<double>{3.0, 4.0, 4.0};
	logicRules->set(logicRulesEventTimes);
	logicRulesMachine.setLogicRules(logicRules);
	logicRulesMachine.updateLogicRules(partitioningTimes);
	logicRulesMachine.display();

	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/

	// Rollout Settings
	Rollout_Settings rolloutSettings;
	rolloutSettings.absTolODE_ = 1e-7;
	rolloutSettings.relTolODE_ = 1e-5;
	rolloutSettings.maxNumStepsPerSecond_ = 10000;

	// rollout class
	using rollout_base_t = RolloutBase<STATE_DIM, INPUT_DIM>;
	std::unique_ptr<rollout_base_t> rolloutBasePtr(new TimeTriggeredRollout<STATE_DIM, INPUT_DIM>(systemDynamics, rolloutSettings));

	rollout_base_t::scalar_array_t timeTrajectory;
	rollout_base_t::size_array_t eventsPastTheEndIndeces;
	rollout_base_t::state_vector_array_t stateTrajectory;
	rollout_base_t::input_vector_array_t inputTrajectory;

	size_t partitionIndex = 0;
	rolloutBasePtr->run(partitionIndex, initTime, initState, 4.0, controller.get(), logicRulesMachine,
			timeTrajectory, eventsPastTheEndIndeces, stateTrajectory, inputTrajectory);

	rolloutBasePtr->display(partitionIndex, timeTrajectory, eventsPastTheEndIndeces, stateTrajectory, &inputTrajectory);

	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/

	// rollout class
//	std::unique_ptr<rollout_base_t> newrolloutBasePtr(new NEWTimeTriggeredRollout<STATE_DIM, INPUT_DIM>(systemDynamics, rolloutSettings));
//
//	newrolloutBasePtr->run(partitionIndex, initTime, initState, 4.0, controller.get(), logicRulesMachine,
//			timeTrajectory, eventsPastTheEndIndeces, stateTrajectory, inputTrajectory);
//
//	newrolloutBasePtr->display(partitionIndex, timeTrajectory, eventsPastTheEndIndeces, stateTrajectory, &inputTrajectory);

}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
