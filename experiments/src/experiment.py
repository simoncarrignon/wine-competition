import json
import os
import time
from string import Template

from .helper import HOMEPATH, mkdirp, save_file, load_file, DISTRIBUTION_BASE_DIR


MAPS_DIR = DISTRIBUTION_BASE_DIR + '/maps'  # This is the place were the benchmark maps reside.
RESULTS_DIR = os.path.abspath(HOMEPATH + '/work/results/decisionMaking')

CONTROLLER_CONFIG = dict(
    mdp='<controller type= "MDP" population="${population}" horizon="${horizon}" width="${width}" '
        'explorationBonus="${bonus}"/>',
    random='<controller type= "random" population="${population}"/>',
    lazy='<controller type= "lazy" population="${population}" alpha="${alpha}"/>',
    greedy='<controller type= "greedy" population="${population}" />',
    motionless='<controller type= "motionless" population="${population}"/>',
    learning='<controller type= "learning" population="${population}" alpha="${alpha}" epsilon="${epsilon}" gamma="${gamma}" episodeLength="${episodeLength}" />',
    sarsa='<controller type= "sarsa" population="${population}" alpha="${alpha}" epsilon="${epsilon}" epsilonDecreaseRate="{epsilonDecreaseRate}" gamma="${gamma}" lambdaParam="${lambdaParam}" episodeLength="${episodeLength}" />',
    evo='<controller type= "evo" population="${population}" evaluationTime="${evaluationTime}" />'
)


class AgentConfiguration(object):
    def __init__(self, population):
        self.population = population

    def get_type(self):
        raise RuntimeError("Method AgentConfiguration.get_type needs to be subclassed")

    def generate_config(self):
        return Template(CONTROLLER_CONFIG[self.get_type()]).substitute(self.__dict__)

    def data(self):
        t = dict(type=self.get_type())
        t.update(self.__dict__)
        return t


class MDPAgentConfiguration(AgentConfiguration):
    def __init__(self, population, width, horizon, bonus=10):
        super(MDPAgentConfiguration,self).__init__(population)
        self.width = width
        self.horizon = horizon
        self.bonus = bonus

    def get_type(self):
        return 'mdp'

class LearningConfiguration(AgentConfiguration):
    def __init__(self, population, epsilon, alpha, gamma, episodeLength):
        super(LearningConfiguration,self).__init__(population)
        self.epsilon = epsilon
        self.alpha = alpha
        self.gamma = gamma
        self.episodeLength = episodeLength

    def get_type(self):
        return 'learning'

class SarsaConfiguration(AgentConfiguration):
    def __init__(self, population, alpha, epsilon, epsilonDecreaseRate, gamma, lambdaParam, episodeLength):
        super(SarsaConfiguration,self).__init__(population)
        self.epsilon = epsilon
        self.epsilonDecreaseRate = epsilonDecreaseRate
        self.alpha = alpha
        self.gamma = gamma
        self.lambdaParam = lambdaParam
        self.episodeLength = episodeLength

    def get_type(self):
        return 'sarsa'

class EvoConfiguration(AgentConfiguration):
    def __init__(self, population, evaluationTime):
        super(EvoConfiguration,self).__init__(population)
        self.evaluationTime = evaluationTime

    def get_type(self):
        return 'evo'


class RandomAgentConfiguration(AgentConfiguration):
    def get_type(self):
        return 'random'


class GreedyAgentConfiguration(AgentConfiguration):
    def get_type(self):
        return 'greedy'


class MotionlessAgentConfiguration(AgentConfiguration):
    def get_type(self):
        return 'motionless'


class LazyAgentConfiguration(AgentConfiguration):
    def __init__(self, population, alpha=1):
        super(LazyAgentConfiguration,self).__init__(population)
        self.alpha = alpha

    def get_type(self):
        return 'lazy'


class SingleExperiment(object):
    """
    A single experiment is an experiment with one single configuration file, but that might have several runs.
    """

    def __init__(self, timesteps, resource_map, obstacle_map, consumption, agent_reproduction, agent_position, label, runs, agents=None):
        self.timesteps = timesteps
        self.obstacle_map = MAPS_DIR + '/' + obstacle_map + '.tif'
        self.resource_map = MAPS_DIR + '/' + resource_map + '.tif'
        self.consumption = consumption
        self.agent_reproduction = agent_reproduction
        self.agent_position = agent_position
        self.runs = runs
        self.agents = agents if agents else []
        self.label = label
        self.directory = None
        self.configs = []

    def add_agent_type(self, config):
        assert isinstance(config, AgentConfiguration)
        self.agents.append(config)

    def bootstrap(self, base_directory):
        self.directory = base_directory + '/' + self.label

        # Create the dirs and write the config files corresponding to each run
        for run in range(self.runs):
            run_dir = self.directory + '/run_' + str(run)
            output_dir = run_dir + '/output'
            logs = run_dir + '/logs'
            mkdirp(output_dir)
            mkdirp(logs)
            config = self.generate_config(
                output=output_dir + '/model.h5',
                logs=logs
            )
            file = run_dir + '/config.xml'
            save_file(file, config)
            self.configs.append(file)

    def generate_config(self, output, logs):
        """ Generates and saves the config.xml file corresponding to the current object parameters """

        tpl = Template(load_file('tpl/config.xml'))

        # Flesh out the template with both the elements of the current object plus the received args
        return tpl.substitute(
            timesteps=self.timesteps,
            mapResource=self.resource_map,
            mapObstacle=self.obstacle_map,
            consumption_factor=self.consumption,
            agent_reproduction="1" if self.agent_reproduction else "0",
            agent_position=self.agent_position,
            output=output,
            logs=logs,
            agent_config='\n\t\t'.join(agent.generate_config() for agent in self.agents)
        )


class AggregateExperiment(object):
    """
    An AggregateExperiment might involve running several SingleExperiments with different configurations,
    e.g. with varying widths of the simulation's MDP agents, etc.
    """

    def __init__(self, args):
        self.timestamp = time.strftime("%Y%m%d_%H%M")
        #self.timeout = args.timeout
        #self.mem = args.mem
        #self.name = '.'.join([args.name, str(args.timeout) + 'sec', '{}GB'.format(args.mem), self.timestamp])
        self.name = '.'.join([self.timestamp, args.name])
        self.directory = RESULTS_DIR + '/' + self.name
        self.singles = []

    def add_single(self, single):
        assert isinstance(single, SingleExperiment)
        self.singles.append(single)

    def bootstrap(self):
        """ Bootstraps an AggregateExperiment by creating the necessary directory structure for its results  """
        # Write all the experiment relevant info
        mkdirp(self.directory)
        save_file(self.directory + '/experiment.json', self.dump())

        print("Bootstrapping experiment " + self.name + " on directory " + self.directory)
        for single in self.singles:
            single.bootstrap(self.directory)

    def __str__(self):
        return "Experiment {}".format(self.name)

    def dump(self):
        def dumper(o):
            return o.data() if hasattr(o, "data") else o.__dict__
        return json.dumps(self, default=dumper, sort_keys=True, indent=4)
