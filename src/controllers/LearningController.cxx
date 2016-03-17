#include "LearningController.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>

namespace Model
{

LearningController::LearningController(const ControllerConfig& config) 
	: _config(config)
	{
		directions = MoveAction::DIRECTIONS;
		cpt = 0;
	}

int LearningController::chooseAction(std::vector<int> state)
{
	int action = 0;
	if(qValues.size() == 0)
	{
		action = Engine::GeneralState::statistics().getUniformDistValue(0, MoveAction::DIRECTIONS.size()-1);
	}
	else
	{
		double maxQ = getQvalue(state,0);
		std::vector<int> bestAction;
		bestAction.push_back(0);
		for(unsigned int i = 1 ; i < directions.size() ; i++)
		{
			double q = getQvalue(state,i);
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

int LearningController::chooseAction(std::vector<int> state, double epsilon)
{
	int action = 0;
	double randomValue = (double)Engine::GeneralState::statistics().getUniformDistValue(0,10000)/10000.0;
	if(randomValue < epsilon)
	{
		action = Engine::GeneralState::statistics().getUniformDistValue(0, MoveAction::DIRECTIONS.size()-1);
	}
	else
	{
		if(qValues.size() == 0)
		{
			action = Engine::GeneralState::statistics().getUniformDistValue(0, MoveAction::DIRECTIONS.size()-1);
		}
		else
		{
			double maxQ = getQvalue(state,0);
			std::vector<int> bestAction;
			bestAction.push_back(0);
			for(unsigned int i = 1 ; i < directions.size() ; i++)
			{
				double q = getQvalue(state,i);
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
	}
	return action;
}


void LearningController::updateQValues(std::vector<int> previousState, int action, double reward, std::vector<int> state)
{
	if (existsQValue(previousState,action) == true)
	{
		double maxFutureQ = getQvalue(state,0);
		for(unsigned int i = 1 ; i < directions.size() ; i++)
		{
			double q = getQvalue(state,i);
			if( q  > maxFutureQ)
			{
				maxFutureQ = q;
			}
		}
			
		double newValue = getQvalue(previousState,action) + _config.alpha * ( reward + _config.gamma * maxFutureQ - getQvalue(previousState,action) );
		setQvalue(previousState,action,newValue);
	}
	else
	{
		setQvalue(previousState,action,reward);
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


std::vector<int> LearningController::computeState(Engine::Point2D<int>& position, Engine::World* world, Engine::DynamicRaster& resourceRaster, Engine::StaticRaster& obstacleRaster)
{
	std::vector<int> state;
	unsigned int num_directions = MoveAction::DIRECTIONS.size();

	for (unsigned int direction = 0 ; direction < num_directions ; direction ++ ) 
	{
		const Engine::Point2D<int> point = position + MoveAction::DIRECTIONS[direction];
		//if the point is within the world and not an obstacle give its energy value
		if ((world->checkPosition(point)) && (obstacleRaster.getValue(point) <= 0)) 
		{
			int res = resourceRaster.getValue(point);
			/*
			if(res > 5)
			{
				state.push_back(1);
			}
			else
			{
				state.push_back(0);
			}
			*/
			state.push_back(res);
		}
		//otherwise just place a -1 for this point. Note: we assume that the energy will never be at -1
		else
		{
			state.push_back(-1);
		}
	}
	return state;
}

Engine::Action* LearningController::selectAction(ModelAgent& agent)
{
	auto current = agent.getPosition();
	Engine::World* world = agent.getWorld();
	assert(world);
	Engine::DynamicRaster resourceRaster = agent.getResourceRaster();
	Engine::StaticRaster obstacleRaster = agent.getObstacleRaster();
	int reward = resourceRaster.getValue(current)-2;

	/*
	cpt ++;
	if(cpt > _config.episodeLength)
	{
		dispQValues(world->getCurrentTimeStep());
		cpt = 0;
	}
	*/

	std::vector<int> state = computeState(current,world,resourceRaster,obstacleRaster);

	updateQValues(previousState,previousAction,reward,state);

	int action = chooseAction(state,_config.epsilon);
	previousAction = action;
	previousState = state;
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

