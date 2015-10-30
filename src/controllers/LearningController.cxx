#include "LearningController.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>

namespace Model
{

LearningController::LearningController(const ControllerConfig& config) 
	: _config(config)
	{
		directions = MoveAction::DIRECTIONS;
	}

int LearningController::chooseAction(std::vector<double> state)
{
	int action = 0;
	if(qValues.size() == 0)
	{
		action = Engine::GeneralState::statistics().getUniformDistValue(0, MoveAction::DIRECTIONS.size());
	}
	else
	{
		double maxQ = getQvalue(state,0);
		std::vector<int> bestAction;
		bestAction.push_back(0);
		for(unsigned int i = 1 ; i < directions.size() ; i++)
		{
			double q = getQvalue(state,0);
			if(q  > maxQ)
			{
				maxQ = q;
				bestAction.clear();
				bestAction.push_back(i);
			}
			if(q == maxQ)
			{
				bestAction.push_back(i);
			}
		}
		int idAction = Engine::GeneralState::statistics().getUniformDistValue(0, bestAction.size());
		action = bestAction[idAction];
	}
	return action;
}


void LearningController::updateQValues(std::vector<double> previousState, int action, double reward, std::vector<double> state)
{
	if (exists(previousState,action) == true)
	{
		double maxFutureQ = getQvalue(state,0);
		for(unsigned int i = 1 ; i < directions.size() ; i++)
		{
			double q = getQvalue(state,0);
			if( q  > maxFutureQ)
			{
				maxFutureQ = q;
			}
		}
			
		double newValue = getQvalue(previousState,action) + _config.alpha * ( reward + _config.gama * maxFutureQ - getQvalue(previousState,action) );
		setQvalue(previousState,action,newValue);
	}
	else
	{
		setQvalue(previousState,action,reward);
	}
}

double LearningController::getQvalue(std::vector<double> state, int action)
{
	auto key = std::make_pair(state,action);
	auto it = qValues.find(key);
	if( it == qValues.end())
	{
		return 0.0;
	}
	else
	{
		return it->second;
	}
}

bool LearningController::exists(std::vector<double> state, int action)
{
	auto key = std::make_pair(state,action);
	auto it = qValues.find(key);
	if( it == qValues.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void LearningController::setQvalue(std::vector<double> state, int action, double value)
{
	auto key = std::make_pair(state,action);
	auto it = qValues.find(key);
	if( it == qValues.end())
	{
		qValues.insert(std::pair<std::pair<std::vector<double>,int>,double>(key,value));
	}
	else
	{
		qValues.at(key) = value;
	}
}


std::vector<double> LearningController::computeState(Engine::Point2D<int>& position, Engine::World* world, Engine::DynamicRaster& raster)
{
	std::vector<double> state;
	unsigned int num_directions = MoveAction::DIRECTIONS.size();

	for (unsigned int direction = 0 ; direction < num_directions ; direction ++ ) 
	{
		const Engine::Point2D<int> point = position + MoveAction::DIRECTIONS[direction];
		if (world->checkPosition(point)) 
		{
			int res = raster.getValue(point);
			state.push_back(res);
		}
	}
	return state;
}

Engine::Action* LearningController::selectAction(ModelAgent& agent)
{
	auto current = agent.getPosition();
	Engine::World* world = agent.getWorld();
	assert(world);
	Engine::DynamicRaster raster = agent.getResourceRaster();

	int reward = raster.getValue(current);

	std::vector<double> state = computeState(current,world,raster);

	updateQValues(previousState,previousAction,reward,state);

	int action = chooseAction(state);
	previousAction = action;
	previousState = state;
	return new MoveAction(action);
}


} // namespaces
