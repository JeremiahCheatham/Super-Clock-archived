# Super Clock
A Binary Clock. Written 3 different ways. Python with PyGame Zero, Python with PyGame and C with SDL2.

Time is displayed in 3 groups of 8 bits for seconds, minutes and hours.

Space - Displays the decimal time in the Window Title for 5 seconds.

S - Changes between Compact, Vertical and Horizontal styles. The style also changes between squares and numbers.

F - Displays actual FPS when launched from the terminal. This is only for the SDL version.

# Python PyGame Zero

PyGame Zero with all dependencies needs to be installed.

To run the program from terminal.

    python superclock-pygame-zero.py


If you wish to install it into your system run this command. This will add a menu entry under Utilities.

    sudo bash ./install-superclock-pygame-zero.sh


You can also run it from the terminal with this command.

    superclock-pygame-zero


# Python PyGame

PyGame with all dependencies needs to be installed.

To run the program from terminal.

    python superclock-pygame.py


If you wish to install it into your system run this command. This will add a menu entry under Utilities.

    sudo bash ./install-superclock-pygame.sh


You can also run it from the terminal with this command.

    superclock-pygame


# C SDL2

SDL2 with all dependencies needs to be installed. You will also need a build enviroment to compile the code.

There is no binary supplied so you will need to compile it.

To only compile the binary run the following.

    make


If you would like to run the file you compliled.

    ./superclock-sdl


If you wish to compile and install it into your system run this command. This will add a menu entry under Utilities.

    sudo bash ./install-superclock-sdl.sh


You can also run it from the terminal with this command.

    superclock

