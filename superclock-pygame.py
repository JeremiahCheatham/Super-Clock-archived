#!/usr/bin/env python

import datetime
import pygame

pygame.init()

TITLE = "Super Clock - PyGame"
ICON = "images/superclock.png"
COLORS = [(20, 20, 20), (223, 223, 223)]
FPS = 60

style = 1
showtime = False
running = True
showtime_timer = pygame.USEREVENT+1


def setup():
    global WIDTH, HEIGHT, rects, screen
    rects = []
    if style == 1 or style == 4:
        WIDTH = 290
        HEIGHT = 180
        for x in [5, 50, 105, 150, 205, 250]:
            for y in [5, 50, 95, 140]:
                rects.append(pygame.Rect((x, y), (35, 35)))
    elif style == 2 or style == 5:
        WIDTH = 155
        HEIGHT = 360
        for x in [5, 60, 115]:
            for y in [5, 50, 95, 140, 185, 230, 275, 320]:
                rects.append(pygame.Rect((x, y), (35, 35)))
    else:
        WIDTH = 360
        HEIGHT = 155
        for y in [5, 60, 115]:
            for x in [5, 50, 95, 140, 185, 230, 275, 320]:
                rects.append(pygame.Rect((x, y), (35, 35)))
    screen = pygame.display.set_mode((WIDTH, HEIGHT))


setup()
pygame.display.set_caption(TITLE)
pygame.display.set_icon(pygame.image.load(ICON).convert_alpha())

font = pygame.font.Font("freesansbold.ttf", 35)
boxes = []
for i in ["0", "1", "2", "3"]:
    surface = pygame.Surface((35, 35))
    if i == "3":
        surface.fill(COLORS[1])
    else:
        surface.fill(COLORS[0])
    if i == "0" or i == "1":
        surface.blit(font.render(i, True, COLORS[1]), (8, 3))
    boxes.append(surface)


def draw():
    global realtime, style
    screen.fill((0, 0, 0))
    if style > 3:
        for i in range(len(rects)):
            screen.blit(boxes[int(realtime[i])], rects[i])
    else:
        for i in range(len(rects)):
            screen.blit(boxes[int(realtime[i]) + 2], rects[i])
    pygame.display.flip()


def update():
    global running, showtime, style, realtime, TITLE

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == showtime_timer:
            showtime = False
            pygame.display.set_caption(TITLE)
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE:
                showtime = True
                pygame.time.set_timer(showtime_timer, 5000, True)
            elif event.key == pygame.K_s:
                if style < 6:
                    style += 1
                else:
                    style = 1
                setup()

    now = datetime.datetime.now()
    realtime = format(now.hour, '08b') \
        + format(now.minute, '08b') + format(now.second, '08b')
    if showtime:
        pygame.display.set_caption(now.strftime("%H:%M:%S"))


while running:
    update()
    draw()
    pygame.time.Clock().tick(FPS)

pygame.quit()
