#!/usr/bin/env bash
if [ 0 != $UID ]; then
    echo "This script must be run as root with sudo.";
    exit 1;
fi

mkdir -p /usr/share/superclock
cp -r images /usr/share/superclock

cp superclock-pygame.py /usr/share/superclock
chmod +x /usr/share/superclock/superclock-pygame.py
cp superclock-pygame /usr/local/bin
chmod +x /usr/local/bin/superclock-pygame
cp superclock-pygame.desktop /usr/share/applications
