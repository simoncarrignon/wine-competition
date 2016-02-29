
#ifndef __Evo_CONTROLLER_HXX__
#define __Evo_CONTROLLER_HXX__

#include "AgentController.hxx"
#include <map>
#include <vector>
#include <Point2D.hxx>
#include <World.hxx>
#include <ModelAgent.hxx>

namespace Model
{

//class EvoState; class EvoProblem; class ControllerConfig;

/**
 * An Evo controller models the surrounding world as a Markov Decision Process, runs UCT
 * for a fixed number of iterations and planning horizon on it,
 * and chooses the action deemed to be optimal.
 */
class EvoController : public AgentController
{
protected:
	//! The configuration object.
	const ControllerConfig _config;

	double realOrientation;
	double realVelocity;
	Engine::Point2D<int> simulatorOrientation;

	void computeInputs(Engine::DynamicRaster raster, Engine::Point2D<int> current);
	
	Engine::Point2D<int> realToSimulatorOrientation(double realInput);

	std::vector<int> inputs;
	std::vector<Engine::Point2D<int>> directions;
	
public:
	EvoController(const ControllerConfig& config);
	virtual ~EvoController() {}
	
	virtual std::string getType() const { return "evo"; }
	virtual ControllerConfig getConfig() const { return _config;} 
	
	virtual Engine::Action* selectAction(ModelAgent& agent);
};

} // namespaces

#endif
