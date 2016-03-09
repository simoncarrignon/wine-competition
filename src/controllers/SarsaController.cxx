#include "SarsaController.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>


namespace Model
{

SarsaController::SarsaController(const ControllerConfig& config) 
	: LearningController(config)
	{
		traces.clear();
		firstStep = true;
	}

int SarsaController::chooseAction(std::vector<int> state)
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

int SarsaController::chooseAction(std::vector<int> state, double epsilon)
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

void SarsaController::updateQValues(std::vector<int> previousState, int previousAction, double reward, std::vector<int> state, int action)
{
	double delta = reward + _config.gamma * getQvalue(state,action) - getQvalue(previousState,previousAction);

	//update or introduce trace
	auto keyTrace = std::make_pair(previousState,previousAction);
	auto itTrace = traces.find(keyTrace);
	if(itTrace == traces.end())
	{
		setTrace(previousState,previousAction,1.0);
	}
	else
	{
		setTrace(previousState,previousAction,getTrace(previousState,previousAction)+1.0);
	}
	
	//introduce new q value if needed
	auto keyQ = std::make_pair(previousState,previousAction);
	auto itQ = qValues.find(keyQ);
	if( itQ == qValues.end())
	{
		setQvalue(previousState,previousAction,0.0);
	}

	//update known q values and traces. The rest remains to zero.
	for (auto it = qValues.begin() ; it != qValues.end() ; it++)
	{
		std::vector<int> itState = std::get<0>(it->first) ;
		int itAction = std::get<1>(it->first) ;
		double newQValue = getQvalue(itState,itAction) + _config.alpha * delta * getTrace(itState, itAction);
		setQvalue(itState,itAction,newQValue);
		double newTrace = _config.gamma * _config.lambda * getTrace(itState,itAction);
		setTrace(itState,itAction,newTrace); 
	}
}

double SarsaController::getTrace(std::vector<int> state, int action)
{
	auto key = std::make_pair(state,action);
	auto it = traces.find(key);
	if( it == traces.end())
	{
		return 0.0;
	}
	else
	{
		return it->second;
	}
}

void SarsaController::setTrace(std::vector<int> state, int action, double value)
{
	std::pair<std::vector<int>,int> key = std::make_pair(state,action);
	std::map<std::pair<std::vector<int>,int>,double>::iterator it = traces.find(key);
	if( it == traces.end())
	{
		traces.insert(std::pair<std::pair<std::vector<int>,int>,double>(key,value));
	}
	else
	{
		traces.at(key) = value;
	}
}

bool SarsaController::existsTrace(std::vector<int> state, int action)
{
	auto key = std::make_pair(state,action);
	auto it = traces.find(key);
	if( it == traces.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

Engine::Action* SarsaController::selectAction(ModelAgent& agent)
{
	auto current = agent.getPosition();
	Engine::World* world = agent.getWorld();
	assert(world);
	Engine::DynamicRaster resourceRaster = agent.getResourceRaster();
	Engine::StaticRaster obstacleRaster = agent.getObstacleRaster();

	stepInEpisode ++;
	int action = 0;

	//new episode
	if((stepInEpisode > _config.episodeLength) or (firstStep == true) or (newEpisode == true))
	{
		//show what the agent has learn
		//dispQValues(world->getCurrentTimeStep());

		//reset all traces
		traces.clear();

		//initialise state and action
		previousState = computeState(current,world,resourceRaster,obstacleRaster);
		previousAction = action;

		if(stepInEpisode > _config.episodeLength)
		{
			agent.setRandomPosition();
			stepInEpisode = 0;
		}
		if (firstStep == true)
		{
			firstStep = false;
		}
		if (newEpisode == true)
		{
			agent.setRandomPosition();
			stepInEpisode = 0;
			newEpisode = false;
		}
	}
	else
	{
		//observe reward and new state
		int reward = -1;
		if (resourceRaster.getValue(current) > 5)
		{
			reward = 100;
			newEpisode = true;
		}
		std::vector<int> state = computeState(current,world,resourceRaster,obstacleRaster);

		//choose action following policy
		action = chooseAction(state,_config.epsilon);

		//update traces and q values
		updateQValues(previousState,previousAction,reward,state,action);

		//update previous states and actions
		previousAction = action;
		previousState = state;
	}
	return new MoveAction(action);
}


} // namespaces

