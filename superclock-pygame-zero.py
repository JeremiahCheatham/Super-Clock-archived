#!/usr/bin/env python

import pgzrun
import datetime
from pgzero.builtins import keys, clock, Rect

TITLE = "Super Clock - PyGame Zero"
ICON = "images/superclock.png"
COLORS = [(20, 20, 20), (223, 223, 223)]

style = 1
showtime = False


def setup():
    global WIDTH, HEIGHT, boxes
    boxes = []
    if style == 1 or style == 4:
        WIDTH = 290
        HEIGHT = 180
        for x in [5, 50, 105, 150, 205, 250]:
            for y in [5, 50, 95, 140]:
                boxes.append(Rect((x, y), (35, 35)))
    elif style == 2 or style == 5:
        WIDTH = 155
        HEIGHT = 360
        for x in [5, 60, 115]:
            for y in [5, 50, 95, 140, 185, 230, 275, 320]:
                boxes.append(Rect((x, y), (35, 35)))
    else:
        WIDTH = 360
        HEIGHT = 155
        for y in [5, 60, 115]:
            for x in [5, 50, 95, 140, 185, 230, 275, 320]:
                boxes.append(Rect((x, y), (35, 35)))


setup()


def hidetime():
    global showtime, TITLE
    showtime = False
    TITLE = "Super Clock - PyGame Zero"


def on_key_down(key):
    global style, showtime
    if key == keys.S:
        if style < 6:
            style += 1
        else:
            style = 1
        setup()
    if key == keys.SPACE:
        showtime = True
        clock.schedule_unique(hidetime, 5.0)


def draw():
    global realtime, style
    screen.clear()
    if style > 3:
        for i in range(len(boxes)):
            screen.draw.filled_rect(boxes[i], COLORS[0])
            screen.draw.textbox(realtime[i], boxes[i], color=COLORS[1])
    else:
        for i in range(len(boxes)):
            screen.draw.filled_rect(boxes[i], COLORS[int(realtime[i])])


def update():
    global realtime, TITLE
    now = datetime.datetime.now()
    realtime = format(now.hour, '08b') \
        + format(now.minute, '08b') + format(now.second, '08b')
    if showtime:
        TITLE = now.strftime("%H:%M:%S")


pgzrun.go()
