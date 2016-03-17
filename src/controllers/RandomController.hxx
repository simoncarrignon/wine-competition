#ifndef __RANDOM_CONTROLLER_HXX__
#define __RANDOM_CONTROLLER_HXX__

#include <Logger.hxx>
#include "AgentController.hxx"
#include <MoveAction.hxx>

namespace Model
{

/**
 * A controller that always chooses one of the nine move actions
 * that are valid for the given agent uniformly at random.
 */
class RandomController : public AgentController
{
protected:
	bool newEpisode;
	int step;
public:
	RandomController() {newEpisode = false;step=0;}
	virtual ~RandomController() {}
	
	virtual Engine::Action* selectAction(ModelAgent& agent) {
		step++;
		if ((newEpisode ==true) or (step == 200))
		{
			agent.setRandomPosition();
			newEpisode = false;
			step = 0;
		}
		Engine::DynamicRaster resourceRaster = agent.getResourceRaster();
		if (resourceRaster.getValue(agent.getPosition()) > 5)
		{
			newEpisode = true;
		}
		return MoveAction::createRandomAction(agent);
	}
	
	virtual std::string getType() const { return "random"; }
};

}

#endif
