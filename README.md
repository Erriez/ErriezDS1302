# Low precision DS1302 RTC library for Arduino

[![Build Status](https://travis-ci.org/Erriez/ErriezDS1302.svg?branch=master)](https://travis-ci.org/Erriez/ErriezDS1302)

This is a 3-wire DS1302 RTC (Real Time Clock) library for Arduino.

![DS1302 RTC](https://raw.githubusercontent.com/Erriez/DS1302/master/extras/DS1302.png)

## Library features

- Read / write RTC date and time.
- Read / write 31 Bytes battery backupped RTC RAM.
- Optimized IO interface for AVR targets (Maximum 169kHz CLK Arduino UNO with 16MHz XTAL).

## DS1302 specifications

**IMPORTANT NOTES:** 

* The DS1302 RTC time may deviate up to 1 minute each day, so this device is not recommended for designs with high precision requirements. 
* Use the high precision DS3231 I2C RTC instead for new designs.
* The 3-wire interface is **NOT** compatible with SPI.

## Examples

Arduino IDE | File | Examples | Erriez DS1302:

- [Alarm](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Alarm/Alarm.ino): Program one or more alarms.
- [Benchmark](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Benchmark/Benchmark.ino): Benchmark library.
- [GettingStarted](https://github.com/Erriez/ErriezDS1302/blob/master/examples/GettingStarted/GettingStarted.ino): Getting started example.
- [PrintDateTime](https://github.com/Erriez/ErriezDS1302/blob/master/examples/PrintDateTime/PrintDateTime.ino): Print date and time with PROGMEM strings.
- [RAM](https://github.com/Erriez/ErriezDS1302/blob/master/examples/RAM/RAM.ino): Read/write RTC RAM.
- [SetDateTime](https://github.com/Erriez/ErriezDS1302/blob/master/examples/SetDateTime/SetDateTime.ino): Set date time.
- [SetTrickleCharger](https://github.com/Erriez/ErriezDS1302/blob/master/examples/SetTrickleCharger/SetTrickleCharger.ino): Program trickle battery/capacitor charger.
- [SquareWave1Hz](https://github.com/Erriez/ErriezDS1302/blob/master/examples/SquareWave1Hz/SquareWave1Hz.ino): 1Hz square wave output on DIGITAL pin.
- [Terminal](https://github.com/Erriez/ErriezDS1302/blob/master/examples/SquareWave1Hz/SquareWave1Hz.ino) [Python](https://github.com/Erriez/ErriezDS1302/blob/master/examples/Terminal/Terminal.py): script to set date time.

## Links

- Library documentation [online](https://erriez.github.io/ErriezDS1302) or [PDF](https://github.com/Erriez/ErriezDS1302/raw/master/docs/latex/refman.pdf).
- [More Libraries and Sketches from Erriez](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches).
- [Wiki](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches/wiki) with [library installation instructions](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches/wiki).

## Usage

**Initialization**

```C++
#include <DS1302.h>

// Connect DS1302 data pin to Arduino DIGITAL pin
#define DS1302_CLK_PIN      2
#define DS1302_IO_PIN       3
#define DS1302_CE_PIN       4

// Create DS1302 RTC object
DS1302 rtc = DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

void setup()
{
    DS1302_DateTime dt;
    
    // Initialize RTC
    rtc.begin();
    
    // Set initial date and time
    dt.second = 0;
    dt.minute = 41;
    dt.hour = 22;
    dt.dayWeek = 6; // 1 = Monday
    dt.dayMonth = 21;
    dt.month = 4;
    dt.year = 2018;
    rtc.setDateTime(&dt);
}

void loop()
{
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

    delay(1000);
}
```

## Benchmark results 

### Arduino UNO (F_CPU = 16MHz)

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

