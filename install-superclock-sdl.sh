#!/usr/bin/env bash
if [ 0 != $UID ]; then
    echo
    echo "This script must be run as root with sudo."
    echo
    exit 1
fi

if ! make rebuild; then
    echo
    echo "Well that didn't work."
    echo "To compile you need a basic biuld enviroment."
    echo "You will also need SDL2 installed."
    echo
    exit 1
fi

if [ ! -f "superclock-sdl" ]; then
    echo
    echo "superclock-sdl file not found."
    echo "Try running the make command to build superclock-sdl."
    echo
    exit 1
fi

mkdir -p /usr/share/superclock
cp -r images /usr/share/superclock

cp freesansbold.ttf /usr/share/superclock
cp superclock /usr/local/bin
cp superclock-sdl /usr/share/superclock
chmod +x /usr/bin/superclock
cp superclock-sdl.desktop /usr/share/applications
