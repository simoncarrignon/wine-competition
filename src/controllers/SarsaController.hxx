#ifndef __Sarsa_CONTROLLER_HXX__
#define __Sarsa_CONTROLLER_HXX__

#include "LearningController.hxx"
#include <map>
#include <vector>
#include <Point2D.hxx>
#include <World.hxx>
#include <ModelAgent.hxx>

namespace Model
{

//class LearningState; class LearningProblem; class ControllerConfig;

/**
 * An Learning controller models the surrounding world as a Markov Decision Process, runs UCT
 * for a fixed number of iterations and planning horizon on it,
 * and chooses the action deemed to be optimal.
 */
class SarsaController : public LearningController
{
protected:
	std::map<std::pair<std::vector<int>,int>, double> traces;

	virtual int chooseAction(std::vector<int> state);

	virtual bool existsTrace(std::vector<int> state, int action);
	double getTrace(std::vector<int> state, int action);
	void setTrace(std::vector<int> state, int action, double value);

	virtual void updateQValues(std::vector<int> previousState, int action, double reward, std::vector<int> state);

	bool firstStep;
	
public:
	SarsaController(const ControllerConfig& config);
	virtual ~SarsaController() {}
	
	virtual std::string getType() const { return "sarsa"; }
	virtual ControllerConfig getConfig() const { return _config;} 
	
	virtual Engine::Action* selectAction(ModelAgent& agent);

	//virtual bool applicable( std::vector<double> state, int) const;
};

} // namespaces

#endif
