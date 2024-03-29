
#include "ModelAgent.hxx"
#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <Environment.hxx>
#include <MDPRaster.hxx>
#include <utils/logging.hxx>

#include "controllers/LearningController.hxx"
#include "controllers/SarsaController.hxx"
#include "controllers/EvoController.hxx"

namespace Model
{

ModelAgent::ModelAgent(unsigned id, Environment* world, AgentController::aptr controller)
	: ModelAgent(controller->getType() + "_" + std::to_string(id), world, controller)
{
}

ModelAgent::ModelAgent(const std::string& id, Environment* world, AgentController::aptr controller)
	: Agent(id), _controller(controller), _numChildren(0)
{
	setWorld(world);
	
	// We can only set the resources here, since initialResources depends on the world being initialized:
	_resources = initialResources();
}


//ModelAgent::~ModelAgent() {}

std::string ModelAgent::getType() {
	return getController()->getType();
}

void ModelAgent::selectActions() {
	assert(_actions.size()==0);
	// We select one single action, and the logic of selection is embedded in the particular instance
	// of the controller
	_actions.push_back(_controller->selectAction(*this)); 
}

void ModelAgent::updateState() {
    
	// Collect the resources from the map
	_resources += collectResources(getResourceRaster(), getPosition()); // Collect the resources
	
	// Consume the resources
	_resources -= dailyResourceConsumption();
	
	// Check for the death of the agent
    if (checkDeathByStarvation(_resources)) {
		remove();
		static_cast<Environment *>(getWorld())->removeAgent(this);
	}
	
	// Check for the reproduction of the agent
    if (checkReproduction(_resources)) {
		reproduceAgent();
	}
}

//! Create an additional agent with a subordinated ID
void ModelAgent::reproduceAgent() {
	++_numChildren;
	std::string id = getId() + "." + std::to_string(_numChildren);
	
	PDEBUG("population", *this << " gives birth to agent " << id);
	
	Environment* world = static_cast<Environment *>(getWorld());
	
	ModelAgent* child;
	if(getType() == "learning") {
		std::shared_ptr<LearningController> dadController = std::dynamic_pointer_cast<LearningController>(getController());
		std::shared_ptr<AgentController> childController;
		childController = std::make_shared<LearningController>(dadController->getConfig());
		child = new ModelAgent(id, world, childController);
	}
	else if(getType() == "sarsa") {
		std::shared_ptr<SarsaController> dadController = std::dynamic_pointer_cast<SarsaController>(getController());
		std::shared_ptr<AgentController> childController;
		childController = std::make_shared<SarsaController>(dadController->getConfig());
		child = new ModelAgent(id, world, childController);
	}
	else if(getType() == "evo") {
		std::shared_ptr<EvoController> dadController = std::dynamic_pointer_cast<EvoController>(getController());
		std::shared_ptr<AgentController> childController;
		childController = std::make_shared<EvoController>(dadController->getConfig());
		child = new ModelAgent(id, world, childController);
	}
	else {
		child = new ModelAgent(id, world, getController()); // We reuse the same controller
	}
	child->setPosition(getPosition()); // The new agent starts at the same position
	
	world->addAgent(child); // Add the agent to the world
	
	// And reduce the number of resources of the current agent
	_resources = consumeResourcesOnReproduction(_resources);
}

// This method has two versions, both should do the same
int ModelAgent::collectResources(Engine::DynamicRaster& resourceRaster, const Engine::Point2D<int>& position) {
	// The agent forages a certain stochastic amount of resources from her destination cell:
	// The amount is drawn uniformly at random between 1 and the total amount of resources available on the cell.
	int availableResources = resourceRaster.getValue(position);
	
	if (availableResources == 0) return 0;
	
	// A small performance optimization to avoid invoking the Random Number Generator if there is only one unit of resources.
	int collectedResources = (availableResources == 1) ? 1 : Engine::GeneralState::statistics().getUniformDistValue(1, availableResources); 
	resourceRaster.setValue(position, availableResources - collectedResources);
	return collectedResources;
}

// This method has two versions, both should do the same
int ModelAgent::collectResources(MDPRaster& resourceRaster, const Engine::Point2D<int>& position) {
	// The agent forages a certain stochastic amount of resources from her destination cell:
	// The amount is drawn uniformly at random between 1 and the total amount of resources available on the cell.
	int availableResources = resourceRaster.at(position);
	
	if (availableResources == 0) return 0;
	
	// A small performance optimization to avoid invoking the Random Number Generator if there is only one unit of resources.
	int collectedResources = (availableResources == 1) ? 1 : Engine::GeneralState::statistics().getUniformDistValue(1, availableResources); 
	resourceRaster.add(position, -collectedResources);
	return collectedResources;
}


void ModelAgent::registerAttributes() {
	registerIntAttribute("resources");
	registerIntAttribute("numChildren");
	if((getType() == "learning") || (getType() == "sarsa")) 
	{
		registerIntAttribute("qValues");
	}
}

void ModelAgent::serialize() {
	serializeAttribute("resources", _resources);
	serializeAttribute("numChildren", _numChildren);
	if(getType() == "learning") 
	{
		std::shared_ptr<LearningController> controller = std::dynamic_pointer_cast<LearningController>(getController());
		serializeAttribute("qValues", controller->getQvalueSize());
	}
	else if (getType() == "sarsa")
	{
		std::shared_ptr<SarsaController> controller = std::dynamic_pointer_cast<SarsaController>(getController());
		serializeAttribute("qValues", controller->getQvalueSize());
	}	
}


//! Helpers to return the agent's world's resource raster, const- and non-const versions
const Engine::DynamicRaster& ModelAgent::getResourceRaster() const {
	return getWorld()->getDynamicRaster(Environment::RESOURCE_RASTER_IDX);
}
Engine::DynamicRaster& ModelAgent::getResourceRaster() {
	return getWorld()->getDynamicRaster(Environment::RESOURCE_RASTER_IDX);
}

//! Helpers to return the agent's world's obstacle raster, const- and non-const versions
const Engine::StaticRaster& ModelAgent::getObstacleRaster() const {
	return getWorld()->getDynamicRaster(Environment::OBSTACLE_RASTER_IDX);
}
Engine::StaticRaster& ModelAgent::getObstacleRaster() {
	return getWorld()->getDynamicRaster(Environment::OBSTACLE_RASTER_IDX);
}

std::ostream& ModelAgent::print(std::ostream& os) {
	os << getId();
	
	os << " [" << getController()->getType() << "]";
	
	if (!exists()) os << " [DEAD!]";
	if (!getWorld()) os << " [DETACHED!]";
	
	os << " [@" << "(" << _position._x << "," << _position._y << ")" << "]";
	os << " [res: " << getResources() << "]";
	return os;
}

} // namespace Model

