/*
 * MIT License
 *
 * Copyright (c) 2018 Erriez
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

/* DS1302 RTC example for Arduino
 *
 * Required library:
 *   https://github.com/Erriez/ErriezDS1302
 */

#include <DS1302.h>

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

static void printDateTime();


void setup()
{
    DS1302_DateTime dt;

    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS1302 RTC set date and time example\n"));

    // Initialize RTC
    rtc.begin();

    // Print current date and time
    Serial.println(F("Current RTC date and time:"));
    printDateTime();

    // Set RTC date and time
    dt.second = 0;
    dt.minute = 35;
    dt.hour = 17;
    dt.dayWeek = 6; // 1 = Monday
    dt.dayMonth = 21;
    dt.month = 4;
    dt.year = 2018;
    rtc.setDateTime(&dt);

    // Check write protect state
    if (rtc.isWriteProtected()) {
        Serial.println(F("Error: DS1302 write protected"));
        while (1) {
            ;
        }
    }

    // Check write protect state
    if (rtc.isHalted()) {
        Serial.println(F("Error: DS1302 halted"));
        while (1) {
            ;
        }
    }

    // Print new date and time
    Serial.println(F("New RTC date and time:"));
    printDateTime();
}

void loop()
{

}

static void printDateTime()
{
    DS1302_DateTime dt;
    char buf[32];

    // Get and print RTC date and time
    if (rtc.getDateTime(&dt)) {
        snprintf(buf, sizeof(buf), "%d %02d-%02d-%d %d:%02d:%02d",
                 dt.dayWeek, dt.dayMonth, dt.month, dt.year, dt.hour, dt.minute, dt.second);
        Serial.println(buf);
    } else {
        Serial.println(F("Error: DS1302 read failed"));
    }
}