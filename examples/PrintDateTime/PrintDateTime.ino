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

#if (defined(__AVR__))
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include <ErriezDS1302.h>

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

const char day_1[] PROGMEM = "Monday";
const char day_2[] PROGMEM = "Tuesday";
const char day_3[] PROGMEM = "Wednesday";
const char day_4[] PROGMEM = "Thursday";
const char day_5[] PROGMEM = "Friday";
const char day_6[] PROGMEM = "Saturday";
const char day_7[] PROGMEM = "Sunday";

const char* const day_week_table[] PROGMEM = {
        day_1, day_2, day_3, day_4, day_5, day_6, day_7
};

const char month_1[] PROGMEM = "January";
const char month_2[] PROGMEM = "February";
const char month_3[] PROGMEM = "March";
const char month_4[] PROGMEM = "April";
const char month_5[] PROGMEM = "May";
const char month_6[] PROGMEM = "June";
const char month_7[] PROGMEM = "July";
const char month_8[] PROGMEM = "August";
const char month_9[] PROGMEM = "September";
const char month_10[] PROGMEM = "October";
const char month_11[] PROGMEM = "November";
const char month_12[] PROGMEM = "December";

const char* const month_table[] PROGMEM = {
        month_1, month_2, month_3, month_4, month_5, month_6,
        month_7, month_8, month_9, month_10, month_11, month_12
};

// Function prototypes
void printDateTime();


void setup()
{
    DS1302_DateTime dt;

    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS1302 RTC print long date and time example\n"));

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

    // Set an initial time and date
    dt.hour = 23;
    dt.minute = 59;
    dt.second = 50;
    dt.dayWeek = 7; // 1 = Monday
    dt.dayMonth = 22;
    dt.month = 4;
    dt.year = 2018;
    // rtc.setDateTime(&dt);
}

void loop()
{
    // Print date and time
    printDateTime();

    // Wait some time
    delay(100);
}

void printDateTime()
{
    static uint8_t lastSec = 60;
    DS1302_DateTime dt;
    char buffer[10];

    // Get RTC date and time
    if (!rtc.getDateTime(&dt)) {
        Serial.println(F("Error: DS1302 read failed"));
        return;
    }

    // Print date and time only when it changed
    if (lastSec == dt.second) {
        return;
    }

    lastSec = dt.second;

    // Print date
    strncpy_P(buffer, (char *)pgm_read_dword(&(day_week_table[dt.dayWeek - 1])), sizeof(buffer));
    Serial.print(buffer);
    Serial.print(F(" "));
    Serial.print(dt.dayMonth);
    Serial.print(F(" "));
    strncpy_P(buffer, (char *)pgm_read_dword(&(month_table[dt.month - 1])), sizeof(buffer));
    Serial.print(buffer);
    Serial.print(F(" "));
    Serial.print(dt.year);
    Serial.print(F("  "));

    // Print time
    snprintf(buffer, sizeof(buffer), "%d:%02d:%02d", dt.hour, dt.minute, dt.second);
    Serial.println(buffer);
}
