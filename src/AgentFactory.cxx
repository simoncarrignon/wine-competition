

#include <AgentFactory.hxx>
#include <ModelAgent.hxx>
#include <EnvironmentConfig.hxx>
#include <controllers/AgentController.hxx>
#include <controllers/MDPController.hxx>
#include <controllers/LazyController.hxx>
#include "controllers/MotionlessController.hxx"
#include "controllers/GreedyController.hxx"
#include "controllers/RuleBasedController.hxx"
#include "controllers/RandomController.hxx"
#include "controllers/LearningController.hxx"
#include "controllers/SarsaController.hxx"
#include "controllers/EvoController.hxx"
#include <Environment.hxx>
#include <memory>

namespace Model 
{

AgentFactory::AgentFactory() : controllers() {}
	
void AgentFactory::registerControllerType(const ControllerConfig& config) {
	createController(config);
}
	
	
ModelAgent* AgentFactory::createAgent(unsigned id, Environment* world, const std::string& type, const ControllerConfig& config) { 
 	if (type == "learning"){
		std::shared_ptr<AgentController> controller;
		controller = std::make_shared<LearningController>(config);
		return new ModelAgent(id,world,controller);
	}
	else if (type == "sarsa") {
		std::shared_ptr<AgentController> controller;
		controller = std::make_shared<SarsaController>(config);
		return new ModelAgent(id,world,controller);
	}
	else if (type == "evo") {
		std::shared_ptr<AgentController> controller;
		controller = std::make_shared<EvoController>(config);
		return new ModelAgent(id,world,controller);
	}
	else {
		return new ModelAgent(id, world, getController(type)); 
	}
}

ModelAgent* AgentFactory::createAgent(const std::string id, Environment* world, const std::string& type, const ControllerConfig& config) { 
 	if (type == "learning"){
		std::shared_ptr<AgentController> controller;
		controller = std::make_shared<LearningController>(config);
		return new ModelAgent(id,world,controller);
	}
	else if (type == "sarsa") {
		std::shared_ptr<AgentController> controller;
		controller = std::make_shared<SarsaController>(config);
		return new ModelAgent(id,world,controller);
	}
	else {
		return new ModelAgent(id, world, getController(type)); 
	}
}
	
	
void AgentFactory::createController(const ControllerConfig& config) {
	std::shared_ptr<AgentController> controller;
	const std::string& type = config.getType();
	
 	if (type == "MDP") {
		controller = std::make_shared<MDPController>(config);
 	} else if (type == "random") {
		controller = std::make_shared<RandomController>();
 	} else if (type == "motionless") {
		controller = std::make_shared<MotionlessController>();
 	} else if (type == "lazy") {
		controller =  std::make_shared<LazyController>(config);
 	} else if (type == "greedy") {
		controller = std::make_shared<GreedyController>();
 	} else if (type == "rule") {
		controller = std::make_shared<RuleBasedController>();
	} else if (type == "learning") {
		controller = std::make_shared<LearningController>(config); //registered for compatibility issue but not used
	} else if (type == "sarsa") {
		controller = std::make_shared<SarsaController>(config); //registered for compatibility issue but not used
 	} else {
 		throw Engine::Exception("Unknown controller type"); // Should never get here!
 	}
	
	controllers.insert(std::make_pair(type, controller));
}


AgentController::aptr AgentFactory::getController(const std::string& type) {
	auto controllerIt = controllers.find(type);
	if (controllerIt != controllers.end()) {
		return controllerIt->second;
	}
	throw Engine::Exception("Tried to instantiate a controller type previously undeclared");
}


} // namespace Model 


