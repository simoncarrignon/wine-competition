#!/usr/bin/python3.4
# -*- coding: utf-8 -*-
import argparse

import itertools
from src.helper import make_filename
from src.experiment import AggregateExperiment, MDPAgentConfiguration, SingleExperiment, LearningConfiguration, SarsaConfiguration, EvoConfiguration, \
	RandomAgentConfiguration, LazyAgentConfiguration, GreedyAgentConfiguration
from src.sge_taskgen import SGETaskgen
from src.sequential_taskgen import SequentialTaskgen
from src.greasy_taskgen import GreasyTaskgen
from src.parallel_taskgen import ParallelTaskgen


def do_experiment(agent_names, args):
    """
        Test the relative performance of different agent types.
    """
    exp = AggregateExperiment(args)

    runs = 10
    population = 10

    autocorrelations = [10]
    #map_instances = list(range(1, 6))  # 5 different map instances
    map_instances = list(range(1, 3))  # 5 different map instances
    consumptions = [1]
    probas = list(range(1,10,1))

    agents = dict(
        mdp=MDPAgentConfiguration(population=population, horizon=8, width=1000),
        lazy=LazyAgentConfiguration(population=population, alpha=0.8),
        random=RandomAgentConfiguration(population=population),
        greedy=GreedyAgentConfiguration(population=population),
        learning = LearningConfiguration(population=population, epsilon = 2 , alpha = 0.2 , gamma = 0.9, episodeLength = 500),
        sarsa = SarsaConfiguration(population=population, epsilon = 0.2 , alpha = 0.01 , gamma = 0.9, lambdaParam = 0.9, episodeLength = 200 ),
        evo = EvoConfiguration(population=population, evaluationTime = 200)
    )

    # Filter out undesired agents
    #agents = {k: v for k, v in agents.items() if k in agent_names}  
    filteredAgents = {}
    for k, v in agents.items(): 
        if k in agent_names:
            filteredAgents[k] = v
    agents=filteredAgents

    for agent_name, agent in agents.items():

        #for autocorrelation, map_instance, consumption in itertools.product(autocorrelations, map_instances, consumptions):
        for autocorrelation, map_instance, proba, consumption in itertools.product(autocorrelations,map_instances,probas,consumptions):

            resource_filename = 'r' + str(autocorrelation) + '_i' + str(map_instance)
            adapt_proba = float(proba) / 10.0
            obstacle_filename = 'obstacle_p' + str(adapt_proba)

            params = dict(timesteps=2000,
                          runs=runs,
                          agent_reproduction=False,
                          agent_position='',
                          resource_map=resource_filename,
                          obstacle_map=obstacle_filename,
                          consumption=consumption,
                          agents=[agent])

            label = make_filename(probaObs=proba,
                                  mapInst=map_instance,
                                  agent=agent_name)

            exp.add_single(SingleExperiment(label=label, **params))

    exp.bootstrap()

    #t = GreasyTaskgen(exp)
    #t = SequentialTaskgen(exp)
    t = ParallelTaskgen(exp)
    t.run()


def parse_arguments(timeout):
    parser = argparse.ArgumentParser(description='Generate experiment task runners.')
    parser.add_argument("--name", required=True, help='The name/ID we want to give to the experiment', default='')
    args = parser.parse_args()
    args.timeout = timeout
    return args


def main():
    #do_experiment(['lazy', 'random', 'greedy','learning','sarsa'], parse_arguments(timeout=100))  # 100sec. are enough for the cheap agents
    do_experiment(['random', 'sarsa', 'evo'], parse_arguments(timeout=100))  # 100sec. are enough for the cheap agents
    #do_experiment(['mdp'], parse_arguments(timeout=12*3600))  # We need much more time for the MDP agent


if __name__ == "__main__":
    main()
