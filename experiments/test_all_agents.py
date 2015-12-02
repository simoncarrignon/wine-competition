#!/usr/bin/python3.4 
# -*- coding: utf-8 -*-
import argparse

import itertools
from src.helper import make_filename
from src.experiment import AggregateExperiment, MDPAgentConfiguration, SingleExperiment, LearningConfiguration, \
	RandomAgentConfiguration, LazyAgentConfiguration, GreedyAgentConfiguration
from src.sge_taskgen import SGETaskgen
from src.sequential_taskgen import SequentialTaskgen
from src.greasy_taskgen import GreasyTaskgen


def do_experiment(agent_names, args):
    """
        Test the relative performance of different agent types.
    """
    exp = AggregateExperiment(args)

    runs = 10
    population = 10

    autocorrelations = [1, 10, 25]
    map_instances = list(range(1, 6))  # 5 different map instances
    consumptions = [2, 3, 4, 5]

    agents = dict(
        mdp=MDPAgentConfiguration(population=population, horizon=8, width=1000),
        lazy=LazyAgentConfiguration(population=population, alpha=0.8),
        random=RandomAgentConfiguration(population=population),
        greedy=GreedyAgentConfiguration(population=population),
        learning = LearningConfiguration(population=1, epsilon = 2 , alpha = 0.2 , gama = 0.9)
    )

    agents = {k: v for k, v in agents.items() if k in agent_names}  # Filter out undesired agents

    for agent_name, agent in agents.items():

        for autocorrelation, map_instance, consumption in itertools.product(autocorrelations, map_instances, consumptions):

            map_filename = 'r{}_i{}'.format(autocorrelation, map_instance)

            params = dict(timesteps=10000,
                          runs=runs,
                          agent_reproduction=True,
                          agent_position='',
                          simulation_map=map_filename,
                          consumption=consumption,
                          agents=[agent])

            label = make_filename(autocorrelation=autocorrelation,
                                  map=map_instance,
                                  consumption=consumption,
                                  agent=agent_name,
                                  run=runs)

            exp.add_single(SingleExperiment(label=label, **params))

    exp.bootstrap()

    t = GreasyTaskgen(exp)
    t.run()


def parse_arguments(timeout):
    parser = argparse.ArgumentParser(description='Generate experiment task runners.')
    parser.add_argument("--name", required=True, help='The name/ID we want to give to the experiment', default='')
    args = parser.parse_args()
    args.timeout = timeout
    return args


def main():
    do_experiment(['lazy', 'random', 'greedy','learning'], parse_arguments(timeout=100))  # 100sec. are enough for the cheap agents
    #do_experiment(['mdp'], parse_arguments(timeout=12*3600))  # We need much more time for the MDP agent


if __name__ == "__main__":
    main()
