# DS1302 RTC (Real Time Clock) library for Arduino

[![Build Status](https://travis-ci.org/Erriez/ErriezDS1302.svg?branch=master)](https://travis-ci.org/Erriez/ErriezDS1302)

This is a 3-wire DS1302 RTC (Real Time Clock) library for Arduino.

![DS1302 RTC](https://raw.githubusercontent.com/Erriez/DS1302/master/extras/DS1302.png)

## Library features

* libc `<time.h>` compatible
* Read/write date/time `struct tm`
* Set/get Unix epoch UTC `time_t`
* Set/get time (hours, minutes, seconds)
* Set/get date and time (hour, min, sec, mday, mon, year, wday)
* Read / write 31 Bytes battery backupped RTC RAM.
* Programmable trickle charge to charge super-caps / lithium batteries.
* Optimized IO interface for Atmel AVR platform.

## DS1302 specifications

**IMPORTANT NOTES:** 

* The DS1302 RTC time may deviate >1 minute each day, so this device is not recommended for designs with high precision requirements. 
* The [high precision DS3231 I2C RTC](https://github.com/Erriez/ErriezDS3231) is recommended for new designs.
* The 3-wire interface is **NOT** compatible with SPI.

## Examples

Arduino IDE | File | Examples | Erriez DS1302 RTC:

* [Alarm](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302Alarm/ErriezDS1302Alarm.ino): Program one or more software alarms
* [Benchmark](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302Benchmark/ErriezDS1302Benchmark.ino): Benchmark library
* [RAM](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302RAM/ErriezDS1302RAM.ino): Read/write RTC RAM.
* [SetBuildDateTime](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302SetBuildDateTime/ErriezDS1302SetBuildDateTime.ino): Set build date/time
* [SetGetDateTime](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302SetGetDateTime/ErriezDS1302SetGetDateTime.ino): Set/get date and time
* [SetGetTime](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302SetGetDateTime/ErriezDS1302SetGetTime.ino): Set/get time
* [SetTrickleCharger](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302SetTrickleCharger/ErriezDS1302SetTrickleCharger.ino): Program trickle battery/capacitor charger
* [Terminal](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302Terminal/ErriezDS1302Terminal.ino) and [Python script](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302Terminal/ErriezDS1302Terminal.py) to set date time
* [Test](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302Test/ErriezDS1302Test.ino): Regression test
* [WriteRead](https://github.com/Erriez/ErriezDS1302/blob/master/examples/ErriezDS1302WriteRead/ErriezDS1302WriteRead.ino): Regression test


## Documentation

* [Online HTML](https://erriez.github.io/ErriezDS1302)
* [Doxygen PDF](https://github.com/Erriez/ErriezDS1302/blob/master/ErriezDS1302.pdf)
* [DS1307 datasheet](https://github.com/Erriez/ErriezDS1302/blob/master/extras/DS1302.pdf)


## Usage

**Initialization**

```c++
#include <ErriezDS1302.h>

// Connect DS1302 data pin to Arduino DIGITAL pin
#if defined(ARDUINO_ARCH_AVR)
#define DS1302_CLK_PIN      2
#define DS1302_IO_PIN       3
#define DS1302_CE_PIN       4
#elif defined(ARDUINO_ARCH_ESP8266)
#define DS1302_CLK_PIN      D4
#define DS1302_IO_PIN       D3
#define DS1302_CE_PIN       D2
#elif defined(ARDUINO_ARCH_ESP32)
#define DS1302_CLK_PIN      0
#define DS1302_IO_PIN       4
#define DS1302_CE_PIN       5
#else
#error #error "May work, but not tested on this target"
#endif

// Create RTC object
ErriezDS1302 rtc = ErriezDS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

void setup()
{
    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }
}
```

**Check oscillator status at startup**

```c++
// Check oscillator status
if (!rtc.isRunning()) {
    // Error: RTC oscillator stopped. Date/time cannot be trusted. 
    // Set new date/time before reading date/time.

    // Enable oscillator
    rtc.clockEnable(true);
}
```

**Set time**

```c++
// Write time to RTC
if (!rtc.setTime(12, 0, 0)) {
    // Error: Set time failed
}
```

**Get time**

```c++
uint8_t hour;
uint8_t minute;
uint8_t second;

// Read time from RTC
if (!rtc.getTime(&hour, &minute, &second)) {
    // Error: RTC read failed
}
```

**Set date and time**

```c++
// Write RTC date/time: 13:45:09  31 December 2019  0=Sunday, 2=Tuesday
if (!rtc.setDateTime(13, 45, 9,  31, 12, 2019,  2) {
    // Error: RTC write failed
}
```

**Get date and time**

```c++
uint8_t hour;
uint8_t min;
uint8_t sec;
uint8_t mday;
uint8_t mon;
uint16_t year;
uint8_t wday;

// Read RTC date/time
if (!rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday) {
    // Error: RTC read failed
}

// hour: 0..23
// min: 0..59
// sec: 0..59
// mday: 1..31
// mon: 1..12
// year: 2000..2099
// wday: 0..6 (0=Sunday .. 6=Saturday)
```

**Write date/time struct tm**

```c++
struct tm dt;

dt.tm_hour = 12;
dt.tm_min = 34;
dt.tm_sec = 56;
dt.tm_mday = 29;
dt.tm_mon = 1; // 0=January
dt.tm_year = 2020-1900;
dt.tm_wday = 6; // 0=Sunday

if (!rtc.write(&dt)) {
    // Error: RTC Read failed
}
```

**Read date/time struct tm**

```c++
struct tm dt;

// Read RTC date/time
if (!rtc.read(&dt)) {
    // Error: RTC read failed
}
```

**Read Unix Epoch UTC**

```c++
time_t t;

// Read Unix epoch UTC from RTC
if (!rtc.getEpoch(&t)) {
    // Error: RTC read failed
}
```

**Write Unix Epoch UTC**

```c++
// Write Unix epoch UTC to RTC
if (!rtc.setEpoch(1599416430UL)) {
    // Error: Set epoch failed
}
```

**Write to RTC RAM**

```c++
// Write Byte to RTC RAM
rtc.writeByteRAM(0x02, 0xA9);

// Write buffer to RTC RAM
uint8_t buf[NUM_DS1302_RAM_REGS] = { 0x00 };
rtc.writeBufferRAM(buf, sizeof(buf));
```

**Read from RTC RAM**

```c++
// Read byte from RTC RAM
uint8_t dataByte = rtc.readByteRAM(0x02);

// Read buffer from RTC RAM
uint8_t buf[NUM_DS1302_RAM_REGS];
rtc.readBufferRAM(buf, sizeof(buf));
```

**Set Trickle Charger**

Please refer to the datasheet how to configure the trickle charger.

```c++
// Disable (default)
rtc.writeRegister(DS1302_REG_TC, DS1302_TCS_DISABLE);

// Minimum 2 Diodes, 8kOhm
rtc.writeRegister(DS1302_REG_TC, 0xAB);

// Maximum 1 Diode, 2kOhm
rtc.writeRegister(DS1302_REG_TC, 0xA5);
```

**Set RTC date and time using Python**

Flash [Terminal](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Terminal/Terminal.ino) example.

Set COM port in [examples/Terminal/Terminal.py](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Terminal/Terminal.py) Python script.

Run Python script:

```c++
// Install Pyserial
python3 pip -m pyserial

// Set RTC date and time
python3 Terminal.py
```

## Pin configuration

**Note:** ESP8266 pin D4 is high during a power cycle / reset / flashing which may corrupt RTC registers. For this reason, pins D2 and D4 are swapped.

| DS1302 Pin | DS1302 IC  |    Atmel AVR    | ESP8266 | ESP32 |
| :--------: | :--------: | :-------------: | :-----: | :---: |
|     4      |    GND     |       GND       |   GND   |  GND  |
|     8      |    VCC2    |  5V (or 3.3V)   |   3V3   |  3V3  |
|     7      | SCLK (CLK) | 2 (DIGITAL pin) |   D4    |   0   |
|     6      | I/O (DAT)  | 3 (DIGITAL pin) |   D2    |   5   |
|     5      |  CE (RST)  | 4 (DIGITAL pin) |   D2    |   4   |

## API changes v1.0.0 to v2.0.0

The API has been changed to make RTC libraries compatible with libc `time.h`. This makes it easier
to calculate with date/time and port the application to different platforms. See changes below:

| v1.0.0                           | v2.0.0                                                       |
| -------------------------------* | -----------------------------------------------------------* |
| `DS1302_DateTime`                | `struct tm`                                                  |
|                                  | `clearOscillatorStopFlag()` merged into `clockEnable()`      |
| `isHalted()`                     | `bool clockEnable(bool enable)`                              |
| `halt()`                         | `void isRunning()`                                           |
| `setDateTime()`                  | `void write(struct tm *dt)`                                  |
| `getDateTime()`                  | `bool read(struct tm *dt)`                                   |
| `getEpochTime()`                 | `time_t getEpoch()`                                          |
|                                  | `void setEpoch(time_t t)`                                    |
| `writeProtect()`                 | Removed                                                      |
| `isProtected()`                  | Removed                                                      |
|                                  | `void setDateTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t mday, uint8_t mon, uint16_t year, uint8_t wday)` |
|                                  | `void getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *mday, uint8_t *mon, uint16_t *year, uint8_t *wday)` |
| `ErriezDS3231Debug`              | class removed to reduce flash size       |


## Library dependencies

* `Wire.h`
* `Terminal.ino` requires `ErriezSerialTerminal` library.


## Library installation

Please refer to the [Wiki](https://github.com/Erriez/ErriezArduinoLibraries/wiki) page.


## More Arduino Libraries from Erriez

* [Erriez Libraries](https://github.com/Erriez/ErriezArduinoLibraries)
