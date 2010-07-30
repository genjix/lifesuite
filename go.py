import array, pygame, random, math
from itertools import cycle

width, height = 640, 480
nodesnn = 10

def render_grid(screen, grid):
    for i in range(nodesnn+1):
        for j in range(nodesnn+1):
            links = grid[i][j]
            toscr = lambda i, j: (i*(width-40-50)/nodesnn + 50, j*(height-40-50)/nodesnn + 50)
            pos = toscr(i, j)
            pygame.draw.circle(screen, (255, 255, 0), pos, 2, 1)
            for dest in links:
                endpos = toscr(*dest)
                pygame.draw.line(screen, (0, 255, 0), pos, endpos, 1)

def solver(grid, cpath, end, depth = 90):
    depth -= 1
    if depth < 0:
        yield None
        raise StopIteration
    neighbours = grid[cpath[-1][0]][cpath[-1][1]]
    execlist = []
    for n in neighbours:
        if n in cpath:
            continue
        if n == end:
            yield cpath + [n]
            raise StopIteration
        execlist.append(solver(grid, cpath + [n], end, depth))
    i = 0
    while len(execlist) > 0:
        i += 1
        if i >= len(execlist):
            i = 0
        try:
            sol = execlist[i].next()
            if sol:
                yield sol
                raise StopIteration
        except StopIteration:
            del execlist[i]
    yield None

screen = pygame.display.set_mode((width, height))
clock = pygame.time.Clock()
running = 1

mousepos, mouseradius, mouse_button = [0,0], 40, 0
grid = []
start, end = None, None
topnt = lambda x, y: (int(round(nodesnn*(x-50)/(width-40-50))), int(round(nodesnn*(y-50)/(height-40-50))))

for i in range(nodesnn+1):
    row = []
    for j in range(nodesnn+1):
        lord = []
        # ordered
        if i > 0:
            lord.append((i-1, j))
        if i < nodesnn:
            lord.append((i+1, j))
        if j > 0:
            lord.append((i, j-1))
        if j < nodesnn:
            lord.append((i, j+1))
        # chaotic
        lcho = []
        for sdkjdsnki in range(4):
            lcho.append((random.randint(0,nodesnn),random.randint(0,nodesnn)))
        links = []
        # both
        try:
            for ooz in range(4):
                fact = random.random()
                mix = lambda a, b: int(round(fact*a + (1-fact)*b))
                a = lord[ooz]
                b = lcho[ooz]
                links.append(map(mix, a, b))
        except IndexError:
            pass
        row.append(links)
    grid.append(row)

while running:
    event = pygame.event.poll()
    if event.type == pygame.QUIT:
        running = 0
    elif event.type == pygame.MOUSEMOTION:
        mousepos = event.pos
    elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
        mouse_button = 1
    elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
        mouse_button = 0
        start = topnt(*mousepos)
        if start[0] < 0 or start[0] > nodesnn or start[1] < 0 or start[1] > nodesnn:
            start = None
    elif event.type == pygame.MOUSEBUTTONUP and event.button == 3:
        mouse_button = 0
        end = topnt(*mousepos)
        if end[0] < 0 or end[0] > nodesnn or end[1] < 0 or end[1] > nodesnn:
            end = None

    if start and end:
        for path in solver(grid, [start], end):
            pass
        print(path)
        start, end = None, None

    screen.fill((0, 0, 0))
    pygame.draw.circle(screen, (255, 255, 0), mousepos, mouseradius, 2)
    render_grid(screen, grid)
    pygame.display.flip()
    clock.tick(240)
