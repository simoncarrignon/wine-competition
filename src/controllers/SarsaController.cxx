#include "SarsaController.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>

namespace Model
{

SarsaController::SarsaController(const ControllerConfig& config) 
	: LearningController(config)
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

void SarsaController::updateQValues(std::vector<int> previousState, int action, double reward, std::vector<int> state)
{
	setTrace(previousState,action,1.0);

	double maxFutureQ = getQvalue(state,0);
	for(unsigned int i = 1 ; i < directions.size() ; i++)
	{
		double q = getQvalue(state,i);
		if( q  > maxFutureQ)
		{
			maxFutureQ = q;
		}
	}

	for (auto it = traces.begin() ; it != traces.end() ; it++)
	{
		traces.at(it->first) = _config.gamma * _config.lambda * it->second;
	}

	for (auto it = qValues.begin() ; it != qValues.end() ; it++)
	{
		std::vector<int> itState = std::get<0>(it->first) ;
		int itAction = std::get<1>(it->first) ;
		double delta = reward + _config.gamma * maxFutureQ - getQvalue(itState,itAction);
		double newValue = getQvalue(itState,itAction) + _config.alpha * delta * getTrace(itState, itAction);
		setQvalue(itState,itAction,newValue);
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

