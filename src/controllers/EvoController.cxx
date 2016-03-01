#include "EvoController.hxx"
#include <EnvironmentConfig.hxx>
#include <MoveAction.hxx>

namespace Model
{

EvoController::EvoController(const ControllerConfig& config) 
	: _config(config)
	{
		realOrientation = 360.0; //TODO initialize randomly
		realVelocity = 1.0;
		rotatedDirections = realToSimulatorOrientation(realOrientation);
	}

std::vector<Engine::Point2D<int>> EvoController::realToSimulatorOrientation(double realInput)
{
	std::vector<Engine::Point2D<int>> directions = MoveAction::DIRECTIONS;
	std::vector<Engine::Point2D<int>> output;
	output.resize(directions.size());
	int shift = 0;
	if(realVelocity > 0.5)
	{
		if((realOrientation >= 337.50) or (realOrientation < 22.50))
		{
			shift = 0;
		}
		else if((realOrientation >= 22.50) and (realOrientation < 67.50))
		{
			shift = 1;
		}
		else if((realOrientation >= 67.50) and (realOrientation < 112.50))
		{
			shift = 2;
		}
		else if((realOrientation >= 112.50) and (realOrientation < 157.50))
		{
			shift = 3;
		}
		else if((realOrientation >= 157.50) and (realOrientation < 202.50))
		{
			shift = 4;
		}
		else if((realOrientation >= 202.50) and (realOrientation < 247.50))
		{
			shift = 5;
		}
		else if((realOrientation >= 247.50) and (realOrientation < 292.50))
		{
			shift = 6;
		}
		else if((realOrientation >= 292.50) and (realOrientation < 337.50))
		{
			shift = 7;
		}
	}
	else
	{
		shift = 0;
	}

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

void EvoController::computeInputs(Engine::DynamicRaster raster, Engine::Point2D<int> current)
{
	inputs.clear();
	rotatedDirections = realToSimulatorOrientation(realOrientation);
	for(unsigned int i = 0 ; i < rotatedDirections.size() ; i ++)
	{
		inputs.push_back(raster.getValue(current+rotatedDirections[i]));
		std::cout << inputs[i] << " ";
	}
	std::cout << std::endl;
}

Engine::Action* EvoController::selectAction(ModelAgent& agent)
{
	agent.setPosition(Engine::Point2D<int>(5,5));
	auto current = agent.getPosition();
	Engine::World* world = agent.getWorld();
	assert(world);
	Engine::DynamicRaster raster = agent.getResourceRaster();
	int reward = raster.getValue(current)-2;

	computeInputs(raster,current);

	return new MoveAction(0);
}



} // namespaces

