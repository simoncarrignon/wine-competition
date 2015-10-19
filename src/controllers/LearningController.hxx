
#ifndef __Learning_CONTROLLER_HXX__
#define __Learning_CONTROLLER_HXX__

#include "AgentController.hxx"
// #include "mdp/LearningState.hxx"
//#include <engine/policy.h>
//#include <engine/uct.h>

namespace Model
{

class LearningState; class LearningProblem; class ControllerConfig;

/**
 * An Learning controller models the surrounding world as a Markov Decision Process, runs UCT
 * for a fixed number of iterations and planning horizon on it,
 * and chooses the action deemed to be optimal.
 */
class LearningController : public AgentController
{
protected:
	//typedef Online::Policy::random_t<LearningState> RandomPolicy;
	//typedef Online::Policy::UCT::uct_t<LearningState> UCTPolicy;
	
	//! The configuration object.
	const ControllerConfig& _config;
	
public:
	LearningController(const ControllerConfig& config);
	virtual ~LearningController() {}
	
	virtual std::string getType() const { return "learning"; }
	
	//! In order to select the appropriate action, we setup an LearningProblem and "solve it" using UCT.
	virtual Engine::Action* selectAction(const ModelAgent& agent) const;
};

} // namespaces

#endif
