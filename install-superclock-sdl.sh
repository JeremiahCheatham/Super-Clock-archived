#!/usr/bin/env bash
if [ 0 != $(id -u) ]; then
    echo "This script must be run as root with sudo.";
    exit 1;
fi

if [ ! -f "superclock-sdl" ]; then
    echo "superclock-sdl file not found.";
    echo "Try running the make command to build superclock-sdl.";
    exit 1;
fi

mkdir -p /usr/share/superclock
cp -r images /usr/share/superclock

cp freesansbold.ttf /usr/share/superclock
cp superclock /usr/bin
cp superclock-sdl /usr/share/superclock
chmod +x /usr/bin/superclock
cp superclock-sdl.desktop /usr/share/applications
