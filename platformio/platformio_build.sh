#!/bin/sh

# Install Python platformio 
# pip install -U platformio

# Update library
# git fetch
# git pull

# Build example(s)
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/Alarm/Alarm.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/Benchmark/Benchmark.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/GettingStarted/GettingStarted.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/PrintDateTime/PrintDateTime.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/RAM/RAM.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/SetTrickleCharger/SetTrickleCharger.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/SquareWave1Hz/SquareWave1Hz.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/Terminal/SquareWave1Hz.ino
