import time
import random
import shelve

import pdb

import sys

import cellular
reload(cellular)
#import qlearn_mod_random as qlearn # to use the alternative exploration method
#import qlearn # to use standard exploration method
#import qlearn_explo as qlearn # to use the alternative exploration method
import sarsa as qlearn # to use the alternative exploration method
reload(qlearn)

directions = 4

lookdist = 1
lookcells = []
for i in range(-lookdist,lookdist+1):
    for j in range(-lookdist,lookdist+1):
        #if (abs(i) + abs(j) <= lookdist) and (i != 0 or j != 0):
        if (i != 0 or j != 0):
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
        self.eaten = 0
        self.fed = 0
        self.lastState = None
        self.lastAction = None
        self.stepsToGoal = 0

    def initAI(self):
        self.states = self.calcAllStates()
        print(len(self.states))
        #for state in self.states:
        #    self.printState(state)
        #    print("###")
        self.ai = qlearn.QLearn(actions=range(directions),states=self.states,
                                #alpha=0.1, gamma=0.9, epsilon=0.1)
                                #alpha=0.1, gamma=0.9, epsilon=2)
                                alpha=0.01, gamma=0.9, lambdaParam=0.9)


    def update(self):
        reward = 0

        state = self.calcState() #the state is basically the view of the world from the agent's perspective
        self.lastState = state

        action = self.ai.chooseAction(state)
        self.goInDirection(action)
        state = self.calcState()

        if self.cell == food.cell:
            self.fed += 1
            reward = 1
        else:
            self.stepsToGoal += 1

        if self.lastState is not None:
            self.ai.learn(self.lastState, action, reward, state)

        if self.cell == food.cell:
            self.cell = pickRandomLocation()
            #self.printQValues()
            self.ai.reset()
            self.stepsToGoal = 0

        self.lastAction = action

    def printQValues(self):
#        for x in range(self.world.width):
#            for y in range(self.world.height):
#                tmpCell = world.getCell(x,y)
#                if tmpCell.wall == False:
#                    state = self.calcState(tmpCell)
#                    for 
        cellsCoord = [(9,2),(9,3),(9,4)]
        for coord in cellsCoord:
            x = coord[0]
            y = coord[1]
            tmpCell = world.getCell(x,y)
            state = self.calcState(tmpCell)
            print(str(x)+","+str(y))
            print(state)
            for action in range(directions):
                q = self.ai.q[(state,action)]
                trace = self.ai.trace[(state,action)]
                print(str(action) + " : " + str(q) + " , " + str(trace))

    def calcAllStates(self):
        allStates = []
        for x in range(self.world.width):
            for y in range(self.world.height):
                tmpCell = world.getCell(x,y)
                if tmpCell.wall == False:
                    state = self.calcState(tmpCell)
                    if not state in allStates:
                        allStates.append(state)
        return allStates


    def calcState(self,cell=None):
        if cell == None:
            cell = self.cell
        def cellvalue(cell):
            if food.cell is not None and (cell.x == food.cell.x and
                                              cell.y == food.cell.y):
                return 2
            else:
                return 1 if cell.wall else 0

        return tuple([cellvalue(self.world.getWrappedCell(cell.x + j, cell.y + i))
                      for i,j in lookcells])

    def printState(self,state):
        if len(state) == 8:
            toPrint = ""
            for printId in range(len(state)):
                if printId == 3:
                    toPrint += str(state[printId]) + " X "
                else:
                    toPrint += str(state[printId]) + " "

                if printId == 2 or printId == 4 or printId == 7:
                    print(toPrint)
                    toPrint = ""
        elif len(state) == 4:
            toPrint = ""
            for printId in range(len(state)):
                if printId == 0 or printId == 3:
                    toPrint += " " + str(state[printId]) 
                else:
                    toPrint += str(state[printId]) + " "
                if printId == 0 or printId == 2 or printId == 3:
                    print(toPrint)
                    toPrint = ""

gatherer = Gatherer()
food = Food()

world = cellular.World(Cell, directions=directions, filename='sutton.txt')
world.age = 0

world.addAgent(food, cell=world.getCell(9,1))
world.addAgent(gatherer, cell=pickRandomLocation())
gatherer.initAI()

endAge = 300000
#endAge = 2000

print("start")
while world.age < endAge:
    world.update()

    if world.age % 10000 == 0:
        if gatherer.ai.epsilon >= 0.0:
            gatherer.ai.epsilon -= 0.01

    if world.age % 10000 == 0:
        print "{:d}, e: {:0.2f}, F: {:d}"\
            .format(world.age, gatherer.ai.epsilon, gatherer.fed)
        gatherer.fed = 0

#q = gatherer.ai.q
#state=[0,0,0,0,0,0,0,0,0,0,0,0]
#ended = False
#while ended == False:
#    for dim in range(12):
#        if state[dim] == 2:
#            state[dim] = 0
#        else:
#            state[dim] += 1
#            break
#
#    ended = True
#    for dim in range(12):
#        if state[dim] != 2:
#            ended = False
#            break
#    
#    for action in range(8):
#        stateActionTuple = (tuple(state),action)
#        if stateActionTuple in q:
#            print(state)
#            print(str(action) + "," + str(q[stateActionTuple]))

gatherer.printQValues()

world.display.activate(size=30)
world.display.delay = 1
while 1:
    world.update()
