
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
	const ControllerConfig& _config;

	std::map<std::pair<std::vector<double>,double>, double> qValues;
	int previousAction;
	std::vector<double> previousState;
	std::vector<Engine::Point2D<int> > directions;

	int chooseAction(std::vector<double> state);

	bool exists(std::vector<double> state, int action);

	double getQvalue(std::vector<double> state, int action);
	void setQvalue(std::vector<double> state, int action, double value);
	void updateQValues(std::vector<double> previousState, int action, double reward, std::vector<double> state);

	std::vector<double> computeState(Engine::Point2D<int>& position, Engine::World* world, Engine::DynamicRaster& raster);

	
public:
	LearningController(const ControllerConfig& config);
	virtual ~LearningController() {}
	
	virtual std::string getType() const { return "learning"; }
	
	virtual Engine::Action* selectAction(ModelAgent& agent);

	//virtual bool applicable( std::vector<double> state, int) const;
};

} // namespaces

#endif
