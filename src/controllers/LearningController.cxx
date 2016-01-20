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

int LearningController::chooseAction(std::vector<int> state)
{
	std::cout << "choose action for ";
	for(unsigned int i = 0 ; i < state.size() ; i++)
	{
		std::cout << state[i] << " ";
	}
	std::cout << std::endl;
	int action = 0;
	if(qValues.size() == 0)
	{
		action = Engine::GeneralState::statistics().getUniformDistValue(0, MoveAction::DIRECTIONS.size()-1);
	}
	else
	{
		double maxQ = getQvalue(state,0);
		std::cout << "0 " << maxQ << std::endl;
		std::vector<int> bestAction;
		bestAction.push_back(0);
		for(unsigned int i = 1 ; i < directions.size() ; i++)
		{
			double q = getQvalue(state,i);
			std::cout << i << " " << q << std::endl;
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
		int idAction = Engine::GeneralState::statistics().getUniformDistValue(0, bestAction.size()-1);
		action = bestAction[idAction];
	}
	return action;
}


void LearningController::updateQValues(std::vector<int> previousState, int action, double reward, std::vector<int> state)
{
	if (existsQValue(previousState,action) == true)
	{
		std::cout << "update q values for previous state" << std::endl;
		double maxFutureQ = getQvalue(state,0);
		for(unsigned int i = 1 ; i < directions.size() ; i++)
		{
			double q = getQvalue(state,i);
			if( q  > maxFutureQ)
			{
				maxFutureQ = q;
			}
		}
		std::cout << "max future q " << maxFutureQ << std::endl;
		std::cout << "previous q " << getQvalue(previousState,action) << std::endl;
		std::cout << "alpha " << _config.alpha << std::endl;
		std::cout << "reward " << reward << std::endl;
		std::cout << "gamma " << _config.gamma << std::endl;
			
		double newValue = getQvalue(previousState,action) + _config.alpha * ( reward + _config.gamma * maxFutureQ - getQvalue(previousState,action) );
		std::cout << "new value " << newValue << std::endl;
		setQvalue(previousState,action,newValue);
	}
	else
	{
		std::cout << "set new q values for previous state: " ;
		setQvalue(previousState,action,reward);
		std::cout << getQvalue(previousState,action) << std::endl;
	}
}

double LearningController::getQvalue(std::vector<int> state, int action)
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

bool LearningController::existsQValue(std::vector<int> state, int action)
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

void LearningController::setQvalue(std::vector<int> state, int action, double value)
{
	std::pair<std::vector<int>,int> key = std::make_pair(state,action);
	std::map<std::pair<std::vector<int>,int>,double>::iterator it = qValues.find(key);
	if( it == qValues.end())
	{
		qValues.insert(std::pair<std::pair<std::vector<int>,int>,double>(key,value));
	}
	else
	{
		qValues.at(key) = value;
	}
}


std::vector<int> LearningController::computeState(Engine::Point2D<int>& position, Engine::World* world, Engine::DynamicRaster& raster)
{
	std::vector<int> state;
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

	int reward = raster.getValue(current)-2;

	std::vector<int> state = computeState(current,world,raster);

	std::cout << world->getCurrentTimeStep() << ": previousState " ;
	for(unsigned int i = 0 ; i < previousState.size() ; i++)
	{
		std::cout << previousState[i] << " ";
	}
	std::cout << std::endl;
	std::cout << world->getCurrentTimeStep() << ": state " ;
	for(unsigned int i = 0 ; i < state.size() ; i++)
	{
		std::cout << state[i] << " ";
	}
	std::cout << std::endl;
	std::cout << world->getCurrentTimeStep() << ": previousAction " << previousAction << std::endl;
	std::cout << world->getCurrentTimeStep() << ": reward " << reward << std::endl;
	updateQValues(previousState,previousAction,reward,state);

	int action = chooseAction(state);
	std::cout << world->getCurrentTimeStep() << ": new action " << action << std::endl;
	previousAction = action;
	previousState = state;
	std::cout << "#####" << std::endl;
	return new MoveAction(action);
}

void LearningController::dispQValues(int timeStep)
{
	std::cout << timeStep << " QValues:" << std::endl;
	//display all known state action couples and their Q values
	for (auto it = qValues.begin() ; it != qValues.end() ; it++)
	{
		std::vector<int> itState = std::get<0>(it->first) ;
		int itAction = std::get<1>(it->first) ;
		double qValue = it->second;
		std::cout << "\t" ;
		for(unsigned int i = 0 ; i < itState.size() ; i++)
		{
			std::cout << itState[i] << " ";
		}
		std::cout << "; " << itAction << " -> " << qValue << std::endl;
	}
}


} // namespaces

