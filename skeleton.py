import array, pygame, random, math

width, height = 640, 480

def render_grid(screen):
    pygame.draw.line(screen, (0, 255, 0), (0,0), (100,100), 2)
    pygame.draw.circle(screen, (255, 255, 0), (200,200), 5, 2)

screen = pygame.display.set_mode((width, height))
clock = pygame.time.Clock()
running = 1

mousepos, mouseradius, mouse_button = [0,0], 40, 0

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

    screen.fill((0, 0, 0))
    pygame.draw.circle(screen, (255, 255, 0), mousepos, mouseradius, 2)
    render_grid(screen)
    pygame.display.flip()
    clock.tick(240)
