rem Install Python27 platformio
rem C:\Python27\Scripts\pip.exe install -U platformio

rem Set Python path
set PATH=%PATH%;C:\Python27
set PATH=%PATH%;C:\Python27\Scripts

rem Update library
rem git fetch
rem git pull

rem Build example(s)
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/Alarm/Alarm.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/Benchmark/Benchmark.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/GettingStarted/GettingStarted.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/PrintDateTime/PrintDateTime.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/RAM/RAM.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/SetTrickleCharger/SetTrickleCharger.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/SquareWave1Hz/SquareWave1Hz.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini ../examples/Terminal/SquareWave1Hz.ino
