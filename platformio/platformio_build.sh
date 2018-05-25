#!/bin/sh

# Install Python platformio 
# pip install -U platformio

# Update library
# git fetch
# git pull

# Build example(s)
platformio ci --lib=".." --lib="../examples/Alarm" --project-conf=platformio.ini ../examples/Alarm/Alarm.ino
platformio ci --lib=".." --lib="../examples/Alarm" --project-conf=platformio.ini ../examples/Benchmark/Benchmark.ino
platformio ci --lib=".." --lib="../examples/Alarm" --project-conf=platformio.ini ../examples/GettingStarted/GettingStarted.ino
platformio ci --lib=".." --lib="../examples/Alarm" --project-conf=platformio.ini ../examples/PrintDateTime/PrintDateTime.ino
platformio ci --lib=".." --lib="../examples/Alarm" --project-conf=platformio.ini ../examples/RAM/RAM.ino
platformio ci --lib=".." --lib="../examples/Alarm" --project-conf=platformio.ini ../examples/SetTrickleCharger/SetTrickleCharger.ino
platformio ci --lib=".." --lib="../examples/Alarm" --project-conf=platformio.ini ../examples/SquareWave1Hz/SquareWave1Hz.ino
platformio ci --lib=".." --lib="../examples/Alarm" --project-conf=platformio.ini ../examples/Terminal/SquareWave1Hz.ino
