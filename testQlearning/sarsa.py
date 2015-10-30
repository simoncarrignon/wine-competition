import random
import sys
import math


class QLearn:
    def __init__(self, actions, states, lambdaParam=0.9, alpha=0.01, gamma=0.9,epsilon=0.1):
        self.q = {}

        self.lambdaParam = lambdaParam
        self.alpha = alpha
        self.gamma = gamma
        self.epsilon = epsilon
        self.actions = actions
        self.states = states

        self.reset()

        self.choice = "greedy"

    def reset(self):
        self.trace = {}
        for state in self.states:
            for action in self.actions:
                self.trace[(state,action)] = 0

    def getQ(self, state, action):
        return self.q.get((state, action), 0.0)
        # return self.q.get((state, action), 1.0)

    def learnQ(self, state, action, reward, value):
        self.trace[(state,action)] = 1.0

        for stateIt in self.states:
            for actionIt in self.actions:
                if stateIt != state and actionIt != action:
                    self.trace[(state,action)] =  self.gamma * self.lambdaParam * self.trace[(state,action)]

        oldv = self.q.get((state, action), None)
        if oldv is None:
            self.q[(state, action)] = reward
        else:
            self.q[(state, action)] = oldv + self.alpha * (value - oldv) * self.trace[(state,action)]


    def chooseAction(self,state):
        if self.choice == "explo":
            return self.exploChoice(self,state)
        elif self.choice == "greedy":
            return self.greedyChoice(self,state)
        else:
            return self.greedyChoice(self,state)

    def greedyChoice(self, state, return_q=False):
        if random.random() < self.epsilon:
            action = random.choice(self.actions)
        else:
            q = [self.getQ(state, a) for a in self.actions]
            maxQ = max(q)
            count = q.count(maxQ)
            if count > 1:
                best = [i for i in range(len(self.actions)) if q[i] == maxQ]
                i = random.choice(best)
            else:
                i = q.index(maxQ)

            action = self.actions[i]
        return action

    def exploChoice(self, state, return_q=False):
        q = [self.getQ(state, a) for a in self.actions]
        q = [math.pow(self.epsilon,self.getQ(state, a)) for a in self.actions]
        sumQ = sum(q)
        if sumQ > 0:
            sum2 = 0.0
            probas = {}
            for a in self.actions:
                sum2 += math.pow(self.epsilon,self.getQ(state, a))
                probas[a] = sum2/sumQ

            r = random.random()
            for a in self.actions:
                action = a
                if (r < probas[a]):
                    break
        else:
            action = random.choice(self.actions)

        if return_q: # if they want it, give it!
            return action, q

        return action

    def learn(self, state1, action1, reward, state2):
        #TODO should modify the learning so as to match the exploration in action
        #see Zhen the best move isn't optimal : Q-learning with exploration. George H. John
        maxqnew = max([self.getQ(state2, a) for a in self.actions])
        self.learnQ(state1, action1, reward, reward + self.gamma*maxqnew)

    def printQ(self):
        keys = self.q.keys()
        states = list(set([a for a,b in keys]))
        actions = list(set([b for a,b in keys]))
        
        dstates = ["".join([str(int(t)) for t in list(tup)]) for tup in states]
        print (" "*4) + " ".join(["%8s" % ("("+s+")") for s in dstates])
        for a in actions:
            print ("%3d " % (a)) + \
                " ".join(["%8.2f" % (self.getQ(s,a)) for s in states])

    def printV(self):
        keys = self.q.keys()
        states = [a for a,b in keys]
        statesX = list(set([x for x,y in states]))
        statesY = list(set([y for x,y in states]))

        print (" "*4) + " ".join(["%4d" % (s) for s in statesX])
        for y in statesY:
            maxQ = [max([self.getQ((x,y),a) for a in self.actions])
                    for x in statesX]
            print ("%3d " % (y)) + " ".join([ff(q,4) for q in maxQ])
        
import math
def ff(f,n):
    fs = "{:f}".format(f)
    if len(fs) < n:
        return ("{:"+n+"s}").format(fs)
    else:
        return fs[:n]
    # s = -1 if f < 0 else 1
    # ss = "-" if s < 0 else ""
    # b = math.floor(math.log10(s*f)) + 1
    # if b >= n:
    #     return ("{:" + n + "d}").format(math.round(f))
    # elif b <= 0:
    #     return (ss + ".{:" + (n-1) + "d}").format(math.round(f * 10**(n-1)))
    # else:
    #     return ("{:"+b+"d}.{:"+(n-b-1)+"
