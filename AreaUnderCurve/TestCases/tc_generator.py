import sys
import os
import random

orig_stdout = sys.stdout

for t in range (1,6):

    path = 'Assignment1/TestCases/y_mixed/testcase'+str(t)
    os.mkdir(path)
    f = open(path+'/input.txt', 'w')
    sys.stdout = f

    low_n=5
    high_n=10
    seed=1000

    n = random.randint(low_n,high_n)

    print(n)

    x=0
    y=0
    dist=0
    old = random.randint(-1*seed, seed)

    for i in range (n):
        dist = random.randint(10,100)
        x = old+dist
        y = random.randint(-1*seed, seed)
        print(x)
        print(y)
        old =x

    f.close()