#!/bin/sh

################################################################################
# Title         : .auto-build.sh
# Date created  : 2 August 2018
__AUTHOR__="Erriez"
#
# This script will start PlatformIO build.
#
################################################################################

################################################################################
##### Setup this script and get the current gh-pages branch.               #####
echo 'Setting up the script...'

# Exit with nonzero exit code if anything fails
set -e

# Build sources
platformio lib -g install https://github.com/Erriez/ErriezDS1302.git
platformio lib -g install https://github.com/Erriez/ErriezTimestamp.git
platformio lib update
platformio ci --lib="examples/Alarm" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 examples/Alarm/Alarm.ino
platformio ci --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 examples/Benchmark/Benchmark.ino
platformio ci --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 examples/GettingStarted/GettingStarted.ino
platformio ci --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 examples/PrintDateTime/PrintDateTime.ino
platformio ci --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 examples/RAM/RAM.ino
platformio ci --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 examples/SetTrickleCharger/SetTrickleCharger.ino
platformio ci --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 examples/SquareWave1Hz/SquareWave1Hz.ino
platformio ci --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 examples/Terminal/Terminal.ino
