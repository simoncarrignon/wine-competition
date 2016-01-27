#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
import argparse

from src.helper import make_filename
# from experiments.src.exp import AggregateExperiment, MDPAgentConfiguration, SingleExperiment
from src.experiment import SingleExperiment, AggregateExperiment, LearningConfiguration, SarsaConfiguration
from src.sge_taskgen import SGETaskgen
from src.sequential_taskgen import SequentialTaskgen
from src.greasy_taskgen import GreasyTaskgen


def main():
    """
    This is a simple example: an experiment executing a number of simulations with learning agents
    """
    exp = AggregateExperiment(parse_arguments())

    for consumption in range(1, 3, 1):
        agent = LearningConfiguration(population=1, epsilon = 0.1 , alpha = 0.1 , gamma = 0.5, episodeLength = 1000)

	label = make_filename(consumption=consumption,
			      agent="learning")

        exp.add_single(SingleExperiment(timesteps=10000, 
                                        consumption=consumption,
                                        agent_reproduction=1,
                                        agent_position="",
                                        simulation_map='map',
                                        label=label,
                                        runs=20,
                                        agents=[agent]))


        agent = SarsaConfiguration(population=1, epsilon = 0.1 , alpha = 0.1 , gamma = 0.1, lambdaParam = 0.9, episodeLength = 500 )

	label = make_filename(consumption=consumption,
			      agent="sarsa")

        exp.add_single(SingleExperiment(timesteps=10000, 
                                        consumption=consumption,
                                        agent_reproduction=1,
                                        agent_position="",
                                        simulation_map='map',
                                        label=label,
                                        runs=20,
                                        agents=[agent]))

    exp.bootstrap()

    #t = GreasyTaskgen(exp)
    t = SequentialTaskgen(exp)
    t.run()


def parse_arguments():
    parser = argparse.ArgumentParser(description='Generate experiment task runners.')
    parser.add_argument("--name", required=True, help='The name/ID we want to give to the experiment', default='')
    args = parser.parse_args()
    return args

if __name__ == "__main__":
    main()
