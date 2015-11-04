#!/usr/bin/python3.4 
# -*- coding: utf-8 -*-
import argparse

# from experiments.src.exp import AggregateExperiment, MDPAgentConfiguration, SingleExperiment
from src.experiment import SingleExperiment, AggregateExperiment, LearningConfiguration
from src.sge_taskgen import SGETaskgen
from src.sequential_taskgen import SequentialTaskgen


def main():
    """
    This is a simple example: an experiment executing a number of simulations with learning agents
    """
    exp = AggregateExperiment(parse_arguments())

    for consumption in range(1, 10, 1):
        agent = LearningConfiguration(population=1, epsilon = 2 , alpha = 0.2 , gama = 0.9)
        exp.add_single(SingleExperiment(timesteps=1000, 
                                        consumption=consumption,
                                        agent_reproduction=1,
                                        agent_position="",
                                        simulation_map='r25_i0',
                                        label="consumption_{}".format(consumption),
                                        runs=10,
                                        agents=[agent]))

    exp.bootstrap()

    t = SequentialTaskgen(exp)
    t.run()


def parse_arguments():
    parser = argparse.ArgumentParser(description='Generate experiment task runners.')
    parser.add_argument("--name", required=True, help='The name/ID we want to give to the experiment', default='')
    args = parser.parse_args()
    return args

if __name__ == "__main__":
    main()
