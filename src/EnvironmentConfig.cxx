

#include <EnvironmentConfig.hxx>
#include <Exception.hxx>
#include <iostream>
#include <tinyxml.h>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace Model
{

const std::vector<std::string> EnvironmentConfig::ALLOWED_CONTROLLERS = {"MDP", "random", "motionless", "lazy", "greedy", "rule", "learning", "sarsa", "evo"};

EnvironmentConfig::EnvironmentConfig(const std::string& filename) : 
	Engine::Config(filename),
	_size(0, 0), mapResource(""), mapObstacle(""), _logdir(""), _controllers(0)
{}

EnvironmentConfig::~EnvironmentConfig()
{}

void EnvironmentConfig::loadParams() {
	// We retrieve the log dir again... no better way of doing it without modifying the base classes.
	_logdir = getParamStr("output", "logsDir");

	// The map size
	_size._width = getParamInt("size", "width");
	_size._height = getParamInt("size", "height");
	
	_consumptionFactor = getParamFloat("resource", "consumptionFactor");
	
	_agentReproduction = getParamBool("agents", "reproduction");
	
	_initialAgentPosition = parseInitialPosition();
	
	// The map raster file
	mapResource = getParamStr("environment", "mapResource");
	mapObstacle = getParamStr("environment", "mapObstacle");
	
	// Load the configuration corresponding to the agent controllers
	loadControllerParams();
}

EnvironmentConfig::PointPtr EnvironmentConfig::parseInitialPosition() {
	//try to get the position. If there is none, leave the string empty
	std::string str = "";
	try
	{
		str = getParamStr("agents", "position");
	}
	catch (Engine::Exception &e)
	{
	}
	
	// If there is no text, return empty pointer.
	if (str == "") return PointPtr(); 
		
	std::vector<std::string> strs;
	boost::split(strs, str, boost::is_any_of(","));
	if (strs.size() != 2) {
		throw new Engine::Exception("Wrong format for the configuration agent initial position");
	}
	return std::make_shared<Engine::Point2D<int>>(boost::lexical_cast<int>(strs[0]), boost::lexical_cast<int>(strs[1]));
}
	
void EnvironmentConfig::loadSingleControllerConfig(TiXmlElement* element) {
	ControllerConfig config;
	config.type = getParamStrFromElem(element, "type");
	config.population = getParamUnsignedFromElem(element, "population");
	
	if (std::find(ALLOWED_CONTROLLERS.begin(), ALLOWED_CONTROLLERS.end(), config.type) == ALLOWED_CONTROLLERS.end()) {
		throw Engine::Exception("Unknown agent controller type '" + config.type + "'");
	}
	
 	if (config.type == "MDP") {
		config.horizon = getParamUnsignedFromElem(element, "horizon");
		config.width = getParamUnsignedFromElem(element, "width");
		config.explorationBonus = getParamFloatFromElem(element, "explorationBonus");
		
		config.explorationBonus = -1 * abs(config.explorationBonus); // the exploration bonus needs to be negative for cost-based (as opposed to reward-based) MDPs.
 	} else if (config.type == "lazy") {
		config.alpha = getParamFloatFromElem(element, "alpha");
	} else if (config.type == "learning") {
		config.alpha = getParamFloatFromElem(element, "alpha");
		config.epsilon = getParamFloatFromElem(element, "epsilon");
		config.gamma = getParamFloatFromElem(element, "gamma");
		config.episodeLength = getParamFloatFromElem(element, "episodeLength");
	} else if (config.type == "sarsa") {
		config.alpha = getParamFloatFromElem(element, "alpha");
		config.epsilon = getParamFloatFromElem(element, "epsilon");
		config.epsilonDecreaseRate = getParamFloatFromElem(element, "epsilonDecreaseRate");
		config.gamma = getParamFloatFromElem(element, "gamma");
		config.lambda = getParamFloatFromElem(element, "lambdaParam");
		config.episodeLength = getParamFloatFromElem(element, "episodeLength");
 	}
	
 	_controllers.push_back(config);
}

void EnvironmentConfig::loadControllerParams() {
	for(TiXmlElement* controller_element = findElement("agents/controller"); controller_element; controller_element = controller_element->NextSiblingElement()) {
		loadSingleControllerConfig(controller_element);
	}
}

} // namespace Model

