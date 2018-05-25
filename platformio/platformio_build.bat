rem Install Python27 platformio
rem C:\Python27\Scripts\pip.exe install -U platformio

rem Set Python path
set PATH=%PATH%;C:\Python27
set PATH=%PATH%;C:\Python27\Scripts

rem Update library
rem git fetch
rem git pull

rem Build example(s)
platformio lib install https://github.com/Erriez/ErriezDS1302.git
platformio lib install https://github.com/Erriez/ErriezTimestamp.git
platformio lib update
platformio ci --lib=".piolibdeps/Erriez DS1302" --lib=".piolibdeps/Erriez Timestamp" --lib="../examples/Alarm" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 ../examples/Alarm/Alarm.ino
platformio ci --lib=".piolibdeps/Erriez DS1302" --lib=".piolibdeps/Erriez Timestamp" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 ../examples/Benchmark/Benchmark.ino
platformio ci --lib=".piolibdeps/Erriez DS1302" --lib=".piolibdeps/Erriez Timestamp" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 ../examples/GettingStarted/GettingStarted.ino
platformio ci --lib=".piolibdeps/Erriez DS1302" --lib=".piolibdeps/Erriez Timestamp" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 ../examples/PrintDateTime/PrintDateTime.ino
platformio ci --lib=".piolibdeps/Erriez DS1302" --lib=".piolibdeps/Erriez Timestamp" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 ../examples/RAM/RAM.ino
platformio ci --lib=".piolibdeps/Erriez DS1302" --lib=".piolibdeps/Erriez Timestamp" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 ../examples/SetTrickleCharger/SetTrickleCharger.ino
platformio ci --lib=".piolibdeps/Erriez DS1302" --lib=".piolibdeps/Erriez Timestamp" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 ../examples/SquareWave1Hz/SquareWave1Hz.ino
platformio ci --lib=".piolibdeps/Erriez DS1302" --lib=".piolibdeps/Erriez Timestamp" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 --board lolin32 ../examples/Terminal/Terminal.ino
