/*
 * MIT License
 *
 * Copyright (c) 2020 Erriez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* DS1302 RTC alarm example for Arduino
 *
 * Required library:
 *   https://github.com/Erriez/ErriezDS1302
 */

#include <ErriezDS1302.h>
#include "ErriezDS1302_Alarm.h"


// Connect DS1302 data pin to Arduino DIGITAL pin
#if defined(ARDUINO_ARCH_AVR)
#define DS1302_CLK_PIN      2
#define DS1302_IO_PIN       3
#define DS1302_CE_PIN       4
#elif defined(ARDUINO_ARCH_ESP8266)
// Swap D2 and D4 pins for the ESP8266, because pin D2 is high during a
// power-on / MCU reset / and flashing. This corrupts RTC registers.
#define DS1302_CLK_PIN      D4 // Pin is high during power-on / reset / flashing
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

// Function prototypes
void printTime(uint8_t hour, uint8_t minute, uint8_t second);


// Alarm on handler
void alarmOn()
{
    Serial.println("Alarm ON");
}

// Alarm off handler
void alarmOff()
{
    Serial.println("Alarm OFF");
}

// Define at least one alarm (hour, minute, second, handler)
Alarm alarms[] = {
        Alarm(12, 0, 5, &alarmOn),
        Alarm(12, 0, 15, &alarmOff),
        Alarm(12, 0, 30, &alarmOn),
        Alarm(12, 1, 0, &alarmOff)
};

void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS1302 RTC alarm example\n"));

    // Initialize RTC
    rtc.begin();

    // Make clock and RAM registers writable
    rtc.writeProtect(false);

    // Enable RTC clock
    rtc.halt(false);

    // Check RTC running and write protect states
    if (rtc.isWriteProtected() || rtc.isHalted()) {
        Serial.println(F("Error: DS1302 not found"));
        while (1) {
            ;
        }
    }

    // Set initial time
    rtc.setTime(12, 0, 0);
}

void loop()
{
    static uint8_t secondLast = 0xff;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    // Read RTC time
    if (!rtc.getTime(&hour, &minute, &second)) {
        Serial.println(F("Error: DS1302 read failed"));
    } else {
        // Print RTC time every second
        if (second != secondLast) {
            secondLast = second;

            // Print RTC time
            printTime(hour, minute, second);

            // Handle alarms
            for (uint8_t i = 0; i < sizeof(alarms) / sizeof(Alarm); i++) {
                alarms[i].tick(hour, minute, second);
            }
        }
    }

    // Wait 100ms
    delay(100);
}

void printTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    char buf[10];

    // Print time
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", hour, minute, second);
    Serial.println(buf);
}
