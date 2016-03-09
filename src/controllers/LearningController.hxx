
#ifndef __Learning_CONTROLLER_HXX__
#define __Learning_CONTROLLER_HXX__

#include "AgentController.hxx"
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
class LearningController : public AgentController
{
protected:
	//! The configuration object.
	const ControllerConfig _config;

	std::map<std::pair<std::vector<int>,int>, double> qValues;
	int previousAction;
	std::vector<int> previousState;
	std::vector<Engine::Point2D<int> > directions;

	virtual int chooseAction(std::vector<int> state);
	virtual int chooseAction(std::vector<int> state, double epsilon);

	virtual bool existsQValue(std::vector<int> state, int action);

	virtual double getQvalue(std::vector<int> state, int action);
	virtual void setQvalue(std::vector<int> state, int action, double value);
	virtual void updateQValues(std::vector<int> previousState, int action, double reward, std::vector<int> state);

	virtual std::vector<int> computeState(Engine::Point2D<int>& position, Engine::World* world, Engine::DynamicRaster& resourceRaster, Engine::StaticRaster& obstacleRaster);

	void dispQValues(int timeStep);

	int cpt;
	
public:
	LearningController(const ControllerConfig& config);
	virtual ~LearningController() {}
	
	virtual std::string getType() const { return "learning"; }
	virtual ControllerConfig getConfig() const { return _config;} 
	
	virtual Engine::Action* selectAction(ModelAgent& agent);

	//virtual bool applicable( std::vector<double> state, int) const;
};

} // namespaces

#endif
