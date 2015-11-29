#include "LearningController.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>

namespace Model
{

SarsaController::SarsaController(const ControllerConfig& config) 
	: LearningControler(config)
	{
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
		int idAction = Engine::GeneralState::statistics().getUniformDistValue(0, bestAction.size()-1);
		action = bestAction[idAction];
	}
	return action;
}

void Sarsa::updateQValues(std::vector<int> previousState, int action, double reward, std::vector<int> state)
{
	setTrace(previousState,action,1.0);

	for (auto it = traces.begin() ; it != traces.end() ; it++)
	{
		traces.at(it) = _config.gamma * _config.lambdaParam * it->second;
	}

	for (auto it = qValues.begin() ; it != qValues.end() ; it++)
	{
		//TODO check how to write the if correctly. Should be state and action of it
		if (existsQvalue(std::get<0>(it->first),std::get<1>(it->first)) == true)
		{
			//TODO write that correctly
			double delta = reward + _config.gama * maxFutureQ - getQvalue(previousState,action);
			double newValue = getQvalue(previousState,action) + _config.alpha * (  );
			setQvalue(previousState,action,newValue);
		}
		//TODO else if it is previousState and action
		{
			setQvalue(previousState,action,reward);
		}
	}
}

double Sarsa::getTrace(std::vector<int> state, int action)
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

void Sarsa::setTrace(std::vector<int> state, int action, double value)
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

bool Sarsa::existsTrace(std::vector<int> state, int action)
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

Engine::Action* LearningController::selectAction(ModelAgent& agent)
{
	auto current = agent.getPosition();
	Engine::World* world = agent.getWorld();
	assert(world);
	Engine::DynamicRaster raster = agent.getResourceRaster();

	int reward = raster.getValue(current);

	std::vector<int> state = computeState(current,world,raster);

	updateQValues(previousState,previousAction,reward,state);

	int action = chooseAction(state);
	previousAction = action;
	previousState = state;
	return new MoveAction(action);
}


} // namespaces

