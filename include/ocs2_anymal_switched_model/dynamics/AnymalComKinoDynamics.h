#ifndef ANYMAL_COMKINODYNAMICS_H_
#define ANYMAL_COMKINODYNAMICS_H_

#include <c_switched_model_interface/dynamics/ComKinoDynamicsBase.h>
#include <c_switched_model_interface/core/Options.h>
#include <c_switched_model_interface/core/MotionPhaseDefinition.h>
#include <ocs2_anymal_switched_model/dynamics/AnymalCom.h>
#include <ocs2_anymal_switched_model/kinematics/AnymalKinematics.h>

namespace anymal {

class AnymalComKinoDynamics : public switched_model::ComKinoDynamicsBase<12>
{
public:
	typedef switched_model::ComKinoDynamicsBase<12> Base;

	AnymalComKinoDynamics(const std::array<bool,4>& stanceLegs,
		const double& gravitationalAcceleration=9.81, const switched_model::Options& options = switched_model::Options(),
		const switched_model::FeetZDirectionPlannerBase::Ptr& feetZDirectionPlanner=NULL,
		const std::vector<switched_model::EndEffectorConstraintBase::Ptr>& endEffectorStateConstraints = std::vector<switched_model::EndEffectorConstraintBase::Ptr>())

	: Base(kinematics_.clone(), comDynamics_.clone(), stanceLegs, gravitationalAcceleration,
			options, feetZDirectionPlanner, endEffectorStateConstraints)
	{}

protected:
	AnymalKinematics kinematics_;
	AnymalCom comDynamics_;

};

} //end of namespace anymal

#endif /* end of include guard: ANYMAL_COMKINODYNAMICS_H_ */
