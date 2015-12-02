from string import Template

from .helper import mkdirp, write_code, load_file, DISTRIBUTION_BASE_DIR
from src import taskgen
import os


HOMEPATH = os.path.expanduser("~")
EXPERIMENTS_BASE_DIR = HOMEPATH + '/work/results/decisionMaking/150922-ComparisonDecisionMaking'
ANALYSIS_BIN = HOMEPATH + '/repos/epnet-dev/150922-ComparisonDecisionMaking/analysis/analysis.bin'

class GreasyTaskgen(taskgen.Taskgen):
    """
    A task generator to run experiments in mare nostrum (using greasy).
    """

    # It is more pratical here to do everything in generate_task. Therefore print_task just passes.
    # And genera_task returns an empty string 

    def generate_task(self):
        configs = [config for single in self.experiment.singles for config in single.configs]

        linking = open("linking.sh","w")
        running = open("running.sh","w")

        for config in configs:
            print(config)
            directory = os.path.dirname(config)
            print(directory)

            linking.write("ln -s" + EXPERIMENTS_BASE_DIR + "/src/simulation.bin " + directory + "\n")
            linking.write("ln -s" + ANALYSIS_BIN + " " + directory + "\n")

            running.write("cd " + directory + " && ./simulation.bin &&  ./analysis.bin " + directory + "/output/model.h5" + " " + directory + "/csv && rm -r " + directory + "/output \n")

        linking.close()
        running.close()

        return ""

    def print_task(self, lines):
        pass
