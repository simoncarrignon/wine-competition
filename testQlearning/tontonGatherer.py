import time
import random
import shelve

import pdb

import cellular
reload(cellular)
#import qlearn_mod_random as qlearn # to use the alternative exploration method
import qlearn # to use standard exploration method
reload(qlearn)

directions = 8

#TODO change the shape of the neighborhood
lookdist = 2
lookcells = []
for i in range(-lookdist,lookdist+1):
    for j in range(-lookdist,lookdist+1):
        if (abs(i) + abs(j) <= lookdist) and (i != 0 or j != 0):
            lookcells.append((i,j))

def pickRandomLocation():
    while 1:
        x = random.randrange(world.width)
        y = random.randrange(world.height)
        cell = world.getCell(x, y)
        if not (cell.wall or len(cell.agents) > 0):
            return cell


class Cell(cellular.Cell):
    wall = False

    def colour(self):
        if self.wall:
            return 'black'
        else:
            return 'white'

    def load(self, data):
        if data == 'X':
            self.wall = True
        else:
            self.wall = False


class Food(cellular.Agent):
    colour = 'yellow'

    def update(self):
        pass


class Gatherer(cellular.Agent):
    colour = 'gray'

    def __init__(self):
        self.ai = None
        self.ai = qlearn.QLearn(actions=range(directions),
                                alpha=0.1, gamma=0.9, epsilon=0.1)
        self.eaten = 0
        self.fed = 0
        self.lastState = None
        self.lastAction = None

    def update(self):
        state = self.calcState()
        reward = -1

        if self.cell == food.cell:
            self.fed += 1
            reward = 50
            food.cell = pickRandomLocation()

        if self.lastState is not None:
            self.ai.learn(self.lastState, self.lastAction, reward, state)

        state = self.calcState()
        action = self.ai.chooseAction(state)
        self.lastState = state
        self.lastAction = action

        self.goInDirection(action)

    def calcState(self):
        def cellvalue(cell):
            if food.cell is not None and (cell.x == food.cell.x and
                                              cell.y == food.cell.y):
                return 2
            else:
                return 1 if cell.wall else 0

        #TODO for now torroidal world. to change
        return tuple([cellvalue(self.world.getWrappedCell(self.cell.x + j, self.cell.y + i))
                      for i,j in lookcells])

gatherer = Gatherer()
food = Food()

world = cellular.World(Cell, directions=directions, filename='waco.txt')
world.age = 0

world.addAgent(food, cell=pickRandomLocation())
world.addAgent(gatherer)

endAge = 150000

while world.age < endAge:
    world.update()

    if world.age % 10000 == 0:
        print "{:d}, e: {:0.2f}, F: {:d}"\
            .format(world.age, gatherer.ai.epsilon, gatherer.fed)
        gatherer.fed = 0

world.display.activate(size=30)
world.display.delay = 1
while 1:
    world.update()
