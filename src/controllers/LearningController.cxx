#include "LearningController.hxx"
//#include "mdp/LearningProblem.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>

namespace Model
{
	

LearningController::LearningController(const ControllerConfig& config) 
	: _config(config)
	{}


Engine::Action* LearningController::selectAction(const ModelAgent& agent) const {
	// The object with all the Learning information and methods.
	// Note that we need to store this object here, since the policy object object keeps only a const reference.
	//LearningProblem problem(_config, agent);
	
	// The policy that we will use as the UCT base policy.
	// For now we are using a random base policy, but other more sophisticated options could also be used.
	// Note that we need to store this object here, since the policy object object keeps only a const reference.		
	//RandomPolicy basePolicy(problem);
	
	// The UCT policy object that we will use to make the decisions.
	//UCTPolicy uctPolicy(basePolicy, _config.getWidth(), _config.getHorizon(), _config.getExplorationBonus(), true );
	
	//Problem::action_t actionIdx = uctPolicy(problem.init());
	//MoveAction::cptr action = problem.init().getApplicableActions().at(actionIdx);
	//TODO not functional
	return new MoveAction(NULL); // The ownership of the action will belong to the Engine::Agent class.
}

} // namespaces

