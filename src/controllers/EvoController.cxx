#include "EvoController.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>

namespace Model
{

EvoController::EvoController(const ControllerConfig& config) 
	: _config(config)
	{
		directions = MoveAction::DIRECTIONS;

		realOrientation = 0.0; //initialize randomly
		simulatorOrientation = realToSimulatorOrientation(realOrientation);
	}

Engine::Point2D<int> EvoController::realToSimulatorOrientation(double realInput)
{
	Engine::Point2D<int> output;
	if(realVelocity > 0.5)
	{
		if((realOrientation >= 337.50) and (realOrientation < 22.50))
		{
			simulatorOrientation = Engine::Point2D<int>(0, 1);
		}
		else if((realOrientation >= 22.50) and (realOrientation < 67.50))
		{
			simulatorOrientation = Engine::Point2D<int>(1, 1);
		}
		else if((realOrientation >= 67.50) and (realOrientation < 112.50))
		{
			simulatorOrientation = Engine::Point2D<int>(1, 0);
		}
		else if((realOrientation >= 112.50) and (realOrientation < 157.50))
		{
			simulatorOrientation = Engine::Point2D<int>(1, -1);
		}
		else if((realOrientation >= 157.50) and (realOrientation < 202.50))
		{
			simulatorOrientation = Engine::Point2D<int>(0, -1);
		}
		else if((realOrientation >= 202.50) and (realOrientation < 247.50))
		{
			simulatorOrientation = Engine::Point2D<int>(-1, -1);
		}
		else if((realOrientation >= 247.50) and (realOrientation < 292.50))
		{
			simulatorOrientation = Engine::Point2D<int>(-1, 0);
		}
		else if((realOrientation >= 292.50) and (realOrientation < 337.50))
		{
			simulatorOrientation = Engine::Point2D<int>(-1, 1);
		}
	}
	else
	{
		simulatorOrientation = Engine::Point2D<int>(0, 0);
	}
	return output;
}

void EvoController::computeInputs(Engine::DynamicRaster raster, Engine::Point2D<int> current)
{
	inputs.clear();
	for(unsigned int i = 0 ; i < directions.size() ; i ++)
	{
		inputs.push_back(raster.getValue(current+realToSimulatorOrientation(realOrientation)+directions[i]));
		std::cout << inputs[i] << std::endl;
	}
}

Engine::Action* EvoController::selectAction(ModelAgent& agent)
{
	auto current = agent.getPosition();
	Engine::World* world = agent.getWorld();
	assert(world);
	Engine::DynamicRaster raster = agent.getResourceRaster();
	int reward = raster.getValue(current)-2;

	return new MoveAction(0);
}



} // namespaces

