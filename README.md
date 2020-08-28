# DS1302 RTC (Real Time Clock) library for Arduino

[![Build Status](https://travis-ci.org/Erriez/ErriezDS1302.svg?branch=master)](https://travis-ci.org/Erriez/ErriezDS1302)

This is an optimized 3-wire DS1302 RTC (Real Time Clock) library for Arduino.

![DS1302 RTC](https://raw.githubusercontent.com/Erriez/DS1302/master/extras/DS1302.png)

## Library features

- Read / write RTC date and time.
- Read / write 31 Bytes battery backupped RTC RAM.
- Programmable trickle charge to charge super-caps / lithium batteries.
- Optimized IO interface for Atmel AVR platform.
- Tested on platforms: 
  - 8-bit Atmel AVR ([Arduino UNO](https://store.arduino.cc/arduino-uno-rev3) / [Nano](https://store.arduino.cc/arduino-nano) / [Mini](https://store.arduino.cc/arduino-mini-05) / [Micro](https://store.arduino.cc/arduino-micro) / [Leonardo](https://store.arduino.cc/arduino-leonardo-with-headers) / [Mega2560](https://store.arduino.cc/arduino-mega-2560-rev3))
  - 32-bit ESP8266 ([WeMos D1 & R2](https://www.google.com/search?q=wemos+d1+r2&tbm=isch) / [Node MCU ESP12E](https://www.google.com/search?q=geekcreit+esp12e+devkit+v2&tbm=isch))
  - 32-bit ESP32 ([WeMos LOLIN32 + OLED](https://www.google.com/search?q=wemos+lolin32+oled&tbm=isch))
- Supported IDE's:
  - [Arduino IDE](https://www.arduino.cc/en/Main/Software) (v1.8.5)
  - [CLion](https://www.jetbrains.com/clion/download) (2018.1)
  - [Atom](https://atom.io/) / [PlatformIO](https://platformio.org/) with CI (Continuous Integration)
  - [Atmel Studio](https://www.microchip.com/mplab/avr-support/atmel-studio-7) (7.0)

## DS1302 specifications

**IMPORTANT NOTES:** 

* The DS1302 RTC time may deviate >1 minute each day, so this device is not recommended for designs with high precision requirements. 
* The [high precision DS3231 I2C RTC](https://github.com/Erriez/ErriezDS3231) is recommended for new designs.
* The 3-wire interface is **NOT** compatible with SPI.

## Examples

Arduino IDE | File | Examples | Erriez DS1302 RTC:

- [Alarm](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Alarm/Alarm.ino): Program one or more alarms.
- [Benchmark](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Benchmark/Benchmark.ino): Benchmark library.
- [GettingStarted](https://github.com/Erriez/ErriezDS1302/blob/master/examples/GettingStarted/GettingStarted.ino): Getting started example.
- [PrintDateTime](https://github.com/Erriez/ErriezDS1302/blob/master/examples/PrintDateTime/PrintDateTime.ino): Print date and time with PROGMEM strings.
- [RAM](https://github.com/Erriez/ErriezDS1302/blob/master/examples/RAM/RAM.ino): Read/write RTC RAM.
- [SetDateTime](https://github.com/Erriez/ErriezDS1302/blob/master/examples/SetDateTime/SetDateTime.ino): Set date time.
- [SetTrickleCharger](https://github.com/Erriez/ErriezDS1302/blob/master/examples/SetTrickleCharger/SetTrickleCharger.ino): Program trickle battery/capacitor charger.
- [SquareWave1Hz](https://github.com/Erriez/ErriezDS1302/blob/master/examples/SquareWave1Hz/SquareWave1Hz.ino): 1Hz square wave output on DIGITAL pin.
- [Terminal](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Terminal/Terminal.ino) and [Python script](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Terminal/Terminal.py) to set date time.


## Documentation

* [Online HTML](https://erriez.github.io/ErriezDS1302)
* [Download PDF](https://github.com/Erriez/ErriezDS1302/raw/master/ErriezDS1302.pdf).
* [DS1302 datasheet](https://www.google.com/search?q=DS1302+datasheet).


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

// Create DS1302 RTC object
DS1302 rtc = DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

void setup()
{
    bool running;
    
    // Initialize RTC
    running = rtc.begin();
}
```

**Set date and time**

```C++
DS1302_DateTime dt;
    
// Set initial date and time
dt.second = 0;
dt.minute = 41;
dt.hour = 22;
dt.dayWeek = 6; // 1 = Monday
dt.dayMonth = 21;
dt.month = 4;
dt.year = 2018;
rtc.setDateTime(&dt);
```

**Get date and time**

```c++
DS1302_DateTime dt;
char buf[32];

// Get RTC date and time
if (!rtc.getDateTime(&dt)) {
    Serial.println(F("Error: DS1302 read failed"));
} else {
    snprintf(buf, sizeof(buf), "%d %02d-%02d-%d %d:%02d:%02d",
             dt.dayWeek, dt.dayMonth, dt.month, dt.year, dt.hour, dt.minute, dt.second);
    Serial.println(buf);
}
```

**Set time**

```c++
// Set time
rtc.setTime(12, 0, 0);
```

**Get time**

```c++
uint8_t hour;
uint8_t minute;
uint8_t second;
char buf[10];

// Read RTC time
if (!rtc.getTime(&hour, &minute, &second)) {
    Serial.println(F("Error: DS1302 read failed"));
} else {    
    // Print time
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", hour, minute, second);
    Serial.println(buf);
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
rtc.writeClockRegister(DS1302_REG_TC, DS1302_TCS_DISABLE);

// Minimum 2 Diodes, 8kOhm
rtc.writeClockRegister(DS1302_REG_TC, 0xAB);

// Maximum 1 Diode, 2kOhm
rtc.writeClockRegister(DS1302_REG_TC, 0xA5);
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

## Benchmark results 

### Arduino UNO (AVR F_CPU = 16MHz)

```
DS1302 RTC benchmark

rtc.begin(): 160us
rtc.writeProtect(false): 148us
rtc.halt(false): 144us
rtc.setDateTime(&dt): 720us
rtc.getDateTime(&dt): 496us
rtc.setTime(12, 0, 0): 1224us
rtc.getTime(&hour, &minute, &second): 272us
rtc.writeRAM(0x00, 0xFF): 144us
rtc.writeRAM(buf, sizeof(buf): 1796us
rtc.readRAM(0x00): 140us
rtc.readRAM(buf, sizeof(buf)): 1812us
```

### WeMos D1 & R2 (ESP8266 F_CPU = 80MHz)

```
DS1302 RTC benchmark

rtc.begin(): 180us
rtc.writeProtect(false): 112us
rtc.halt(false): 149us
rtc.setDateTime(&dt): 369us
rtc.getDateTime(&dt): 273us
rtc.setTime(12, 0, 0): 571us
rtc.getTime(&hour, &minute, &second): 154us
rtc.writeRAM(0x00, 0xFF): 86us
rtc.writeRAM(buf, sizeof(buf): 852us
rtc.readRAM(0x00): 84us
rtc.readRAM(buf, sizeof(buf)): 881us
```

### WeMos D1 & R2 (ESP8266 F_CPU = 160MHz)

```
DS1302 RTC benchmark

rtc.begin(): 152us
rtc.writeProtect(false): 73us
rtc.halt(false): 108us
rtc.setDateTime(&dt): 257us
rtc.getDateTime(&dt): 187us
rtc.setTime(12, 0, 0): 373us
rtc.getTime(&hour, &minute, &second): 105us
rtc.writeRAM(0x00, 0xFF): 62us
rtc.writeRAM(buf, sizeof(buf): 553us
rtc.readRAM(0x00): 62us
rtc.readRAM(buf, sizeof(buf)): 568us
```

## Library installation

Please refer to the [Wiki](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches/wiki) page.


## Other Arduino Libraries and Sketches from Erriez

* [Erriez Libraries and Sketches](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches)
