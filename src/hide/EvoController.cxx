#include "EvoController.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>
#include <math.h>

#include <cmaes.h>
#include "OnlineCMAStrategy.hxx"

using namespace libcmaes;

FitFunc computeFitness = [](const double *x, const int N)
{
	//sum up the fitnesses computed
  double val = 0.0;
  for (int i=0;i<N;i++)
	{
    val += x[i];
	}

	//turn the problem as a minimization
	std::cout << "val " << val ; 
	val = -log(val+1);
	std::cout << " " << val << std::endl;
  return val;
};

namespace Model
{

EvoController::EvoController(const ControllerConfig& config) 
	: _config(config)
	{
		step = 0;

		hiddenLayerSize = 5 ; //TODO load hidden layer size from configuration
		realOrientation = 45.0; //TODO initialize randomly
		rotationSpeed = 10.0; //TODO get it from configuration
		realVelocity = 0.0;
		paramSize = ((MoveAction::DIRECTIONS.size() + 1) * hiddenLayerSize ) + (hiddenLayerSize *2);

		//evolution parameters
		std::vector<double> x0(paramSize,0.1);
		double sigma = 0.5;
		evaluationTime = 500; //TODO get it from configuration

		//initialize CMA-ES
		CMAParameters<> cmaparams(x0,sigma);
		cmaparams.set_uh(true);
		optim = ESOptimizer<OnlineCMAStrategy,CMAParameters<>>(computeFitness,cmaparams);
		optim.init(evaluationTime);

		//initalize the parameters of the controller
		params = optim.getParams();
	}

int EvoController::angleToDirection(double angle)
{
	int directionIndice = 0;

	if((angle >= 337.50) or (angle < 22.50))
	{
		directionIndice = 0;
	}
	else if((angle >= 22.50) and (angle < 67.50))
	{
		directionIndice = 1;
	}
	else if((angle >= 67.50) and (angle < 112.50))
	{
		directionIndice = 2;
	}
	else if((angle >= 112.50) and (angle < 157.50))
	{
		directionIndice = 3;
	}
	else if((angle >= 157.50) and (angle < 202.50))
	{
		directionIndice = 4;
	}
	else if((angle >= 202.50) and (angle < 247.50))
	{
		directionIndice = 5;
	}
	else if((angle >= 247.50) and (angle < 292.50))
	{
		directionIndice = 6;
	}
	else if((angle >= 292.50) and (angle < 337.50))
	{
		directionIndice = 7;
	}

	return directionIndice;
}

std::vector<Engine::Point2D<int>> EvoController::realToSimulatorOrientation(double realInput)
{
	//the view will be recentered on the current direction of the agent
	int shift = angleToDirection(realOrientation);

	std::vector<Engine::Point2D<int>> directions = MoveAction::DIRECTIONS;
	std::vector<Engine::Point2D<int>> output;
	output.resize(directions.size());

	output[0] = directions[0];
	for(unsigned int i = 1 ; i < directions.size() ; i ++)
	{
		unsigned int j = i+shift;
		if (j >= directions.size())
		{
			j = j-directions.size() + 1;
		}
		output[i] = directions[j];
	}

	return output;
}

void EvoController::computeInputs(Engine::World* world, Engine::DynamicRaster raster, Engine::Point2D<int> current)
{
	inputs.clear();

	std::vector<Engine::Point2D<int>> rotatedDirections = realToSimulatorOrientation(realOrientation);
	for(unsigned int i = 0 ; i < rotatedDirections.size() ; i ++)
	{
		const Engine::Point2D<int> point = current + rotatedDirections[i];
		if (world->checkPosition(point)) 
		{
			inputs.push_back(raster.getValue(current+rotatedDirections[i])/10.0);
		}
		else
		{
			inputs.push_back(-1);
		}
	}
}

void EvoController::computeOutputs()
{
	int paramsIndice = 0;

	//from inputs to hidden layer

	// weighted sum
	std::vector<double> tmp(hiddenLayerSize, 0.0);
	for(unsigned int i = 0; i < inputs.size(); i++) {
		for(unsigned int j = 0; j < tmp.size(); j++) {
			tmp[j] += inputs[i] * params[paramsIndice++];
		}
	}

	//use of bias
	for(unsigned int j = 0; j < tmp.size(); j++)
		tmp[j] += params[paramsIndice++] * 1.0;

	// Tanh activation function
	for(unsigned int i = 0; i < tmp.size(); i++)
		tmp[i] = tanh(tmp[i]);

	//from hidden layer to output

	// weighted sum
	std::vector<double> outputs(2, 0.0);
	for(unsigned int i = 0; i < tmp.size(); i++) {
		for(unsigned int j = 0; j < 2; j++) {
			outputs[j] += tmp[i] * params[paramsIndice++];
		}
	}

	//do not use a new bias from hidden to output

	// Tanh activation function
	for(unsigned int i = 0; i < 2; i++)
		outputs[i] = tanh(outputs[i]);

	//use output to update orientation
	realOrientation = realOrientation + (outputs[0] * rotationSpeed);
	//replace the orientation between 0 and 360.
	//use fmod to have it with double
	realOrientation = fmod(realOrientation,360);

	//use output to update velocity and cast it between -1 and 1
	realVelocity = realVelocity + outputs[1];
	if (realVelocity > 1.0)
	{
		realVelocity = 1.0;
	}
	if (realVelocity < -1.0)
	{
		realVelocity = -1.0;
	}
}

Engine::Action* EvoController::computeAction()
{
	if (realVelocity > 0.5)
	{
		return new MoveAction(angleToDirection(realOrientation));
	}
	return new MoveAction(0);
}


void EvoController::evoStep(double reward)
{
  if(!optim.stop())
	{
		//set the fitness
		optim.setFitness(step, reward);

		//if we have reach  the end of evaluation time, ask to CMA-ES to do its job
		if((step % evaluationTime) == 0)
		{
			std::cout << "params " ;
			for(unsigned int i = 0 ; i < params.size() ; i++)
			{
				std::cout << params[i] << " ";
			}
			std::cout << std::endl;
			optim.completeEvaluation();
		}
	}
	//get the parameters from CMA-ES
	params = optim.getParams();
}

Engine::Action* EvoController::selectAction(ModelAgent& agent)
{
	step++;

	auto current = agent.getPosition();
	Engine::World* world = agent.getWorld();
	assert(world);
	Engine::DynamicRaster raster = agent.getResourceRaster();
	int reward = raster.getValue(current);
	//std::cout << "reward " << reward << std::endl;
	evoStep(reward);

	computeInputs(world,raster,current);
	computeOutputs();
	return computeAction();
	//return new MoveAction(0);
}



} // namespaces

