# use python 3
from itertools import product, repeat
import random

def lifestep(grid):
    # find all vectors going from north = (0,-1) to north-east = (1,-1), ... to north-west = (-1,-1)
    surround = lambda: filter(lambda n: n != (0, 0), product((-1, 0, 1), repeat=2))
    # add two points together
    addpnts = lambda p, q: (p[0] + q[0], p[1] + q[1])
    # offset (x,y) with the vectors in all directions
    locations = lambda x, y: map(addpnts, repeat((x, y)), surround())
    # filter out of bounds locations
    validlocs = lambda arr, x, y: filter(lambda p: max(p) < len(arr) and min(p) >= 0, locations(x, y))
    # lookup the cells and count the number of live cells
    neighbours = lambda arr, x, y: list(map(lambda p: arr[p[1]][p[0]], validlocs(arr, x, y))).count(1)

    nexgrid = [[0 for col in row] for row in grid]
    for y, row in enumerate(grid):
        for x, state in enumerate(row):
            # number of neighbours
            n = neighbours(grid, x, y)

            # alive cell
            if state == 1:
                # allowed to live
                if 2 <= n <= 3:
                    ns = 1
                # isolation or overcrowding
                else:
                    ns = 0
            # dead cell
            elif state == 0:
                # reproduction
                if n == 3:
                    ns = 1
                # stays dead
                else:
                    ns = 0
            nexgrid[y][x] = ns
    return nexgrid

if __name__ == "__main__":
    import sys

    blinker = [[0, 0, 0, 0, 0],
               [0, 0, 1, 0, 0],
               [0, 0, 1, 0, 0],
               [0, 0, 1, 0, 0],
               [0, 0, 0, 0, 0]]

    beacon = [[0, 0, 0, 0, 0, 0],
              [0, 1, 1, 0, 0, 0],
              [0, 1, 1, 0, 0, 0],
              [0, 0, 0, 1, 1, 0],
              [0, 0, 0, 1, 1, 0],
              [0, 0, 0, 0, 0, 0]]

    grid = [[round(random.random()) for cell in range(50)] for row in range(50)] 

    while True:
        nex = lifestep(grid)
        for row in nex:
            for cell in row:
                if cell == 1:
                    sys.stdout.write("oo")
                elif cell == 0:
                    sys.stdout.write("  ")
            print()
        grid = nex
        #input()
