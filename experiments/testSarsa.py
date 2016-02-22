#!/usr/bin/python3.4 
# -*- coding: utf-8 -*-
import argparse

from src.helper import make_filename
# from experiments.src.exp import AggregateExperiment, MDPAgentConfiguration, SingleExperiment
from src.experiment import SingleExperiment, AggregateExperiment, LearningConfiguration, SarsaConfiguration
from src.sge_taskgen import SGETaskgen
from src.sequential_taskgen import SequentialTaskgen


def main():
    """
    This is a simple example: an experiment executing a number of simulations with learning agents
    """
    exp = AggregateExperiment(parse_arguments())

    for consumption in [2]:
        for alpha in [0.05,0.25] :
            for epsilon in [0.05,0.25]:
                for gamma in [0.05,0.25]:
                    for lambdaParam in [0.05,0.15]:
                        for episodeLength in [200,500]:

                            agent = SarsaConfiguration(population=1, alpha = alpha, epsilon = epsilon, gamma = gamma, lambdaParam = lambdaParam, episodeLength = episodeLength )

                            label = make_filename(consumption=consumption,
                                                  alpha = alpha,
                                                  epsilon = epsilon,
                                                  gamma = gamma,
                                                  lambdaParam = lambdaParam,
                                                  episodeLength = episodeLength,
                                                  agent="sarsa")

                            exp.add_single(SingleExperiment(timesteps=1000, 
                                                            consumption=consumption,
                                                            agent_reproduction=1,
                                                            agent_position="",
                                                            simulation_map='r25_i0',
                                                            label=label,
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
