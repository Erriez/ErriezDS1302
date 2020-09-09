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

/*!
 * \brief DS1302 RTC set date/time example for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS1302
 *    Documentation:  https://erriez.github.io/ErriezDS1302
 */

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
ErriezDS1302 rtc = ErriezDS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

// Global date/time object
struct tm dt;


void rtcInit()
{
    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Enable RTC clock
    rtc.clockEnable(true);
}

bool getBuildTime(const char *str)
{
    int hour;
    int minute;
    int second;

    // Convert build time macro to time
    if (sscanf(str, "%d:%d:%d", &hour, &minute, &second) != 3) {
        return false;
    }

    dt.tm_hour = hour;
    dt.tm_min = minute;
    dt.tm_sec = second;

    return true;
}

bool getBuildDate(const char *str)
{
    const char *monthName[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    uint8_t monthIndex;
    char month[12];
    int dayMonth;
    int year;
    time_t t;

    // Convert build time macro to date
    if (sscanf(str, "%s %d %d", month, &dayMonth, &year) != 3) {
        return false;
    }

    // Convert month string to month number
    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
        if (strcmp(month, monthName[monthIndex]) == 0) {
            break;
        }
    }
    if (monthIndex >= 12) {
        // Not found
        return false;
    }

    // Set date
    dt.tm_mday = dayMonth;
    dt.tm_mon = monthIndex;
    dt.tm_year = year - 1900;

    // Calculate day of the week
    t = mktime(&dt);
    dt.tm_wday = localtime(&t)->tm_wday;

    return true;
}

bool rtcSetDateTime()
{
    // Convert compile date/time to date/time string
    if (!getBuildDate(__DATE__) || !getBuildTime(__TIME__)) {
        Serial.print(F("Build date/time error"));
        return false;
    }

    // Print build date/time
    Serial.print(F("Build date time: "));
    Serial.println(asctime(&dt));

    // Set new date time
    Serial.print(F("Set RTC date time..."));
    if (!rtc.write(&dt)) {
        Serial.println(F("FAILED"));
    } else {
        Serial.println(F("OK"));
    }

    return true;
}

void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1302 RTC Set Time example\n"));

    // Initialize RTC
    rtcInit();

    // Set date/time
    if (!rtcSetDateTime()) {
        // Could not parse build date/time to program RTC
        while (1) {
            delay(1000);
        }
    }
}

void loop()
{
    // Get date/time
    if (!rtc.read(&dt)) {
        Serial.println(F("RTC read failed"));
    } else {
        Serial.println(asctime(&dt));
    }

    // Wait some time
    delay(1000);
}