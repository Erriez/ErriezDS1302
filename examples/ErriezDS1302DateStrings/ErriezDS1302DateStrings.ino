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
 * \brief DS1302 high accurate RTC getting started example for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS1302
 *    Documentation:  https://erriez.github.io/ErriezDS1302
 *
 *    Connect the nINT/SQW pin to an Arduino interrupt pin
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
ErriezDS1302 ds1302 = ErriezDS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);


// Create date time object
struct tm dt;

// Define days of the week in flash
const char day_0[] PROGMEM = "Sunday";
const char day_1[] PROGMEM = "Monday";
const char day_2[] PROGMEM = "Tuesday";
const char day_3[] PROGMEM = "Wednesday";
const char day_4[] PROGMEM = "Thursday";
const char day_5[] PROGMEM = "Friday";
const char day_6[] PROGMEM = "Saturday";

const char* const day_week_table[] PROGMEM = {
    day_0, day_1, day_2, day_3, day_4, day_5, day_6
};

// Define months in flash
const char month_0[] PROGMEM = "January";
const char month_1[] PROGMEM = "February";
const char month_2[] PROGMEM = "March";
const char month_3[] PROGMEM = "April";
const char month_4[] PROGMEM = "May";
const char month_5[] PROGMEM = "June";
const char month_6[] PROGMEM = "July";
const char month_7[] PROGMEM = "August";
const char month_8[] PROGMEM = "September";
const char month_9[] PROGMEM = "October";
const char month_10[] PROGMEM = "November";
const char month_11[] PROGMEM = "December";

const char* const month_table[] PROGMEM = {
    month_0, month_1, month_2, month_3, month_4, month_5,
    month_6, month_7, month_8, month_9, month_10, month_11
};


void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1302 RTC getting started example\n"));

    // Initialize RTC
    while (!ds1302.begin()) {
        Serial.println(F("Error: Could not detect DS1302 RTC"));
        delay(3000);
    }

    // Enable RTC clock
    ds1302.clockEnable(true);
}

void loop()
{
    char buf[32];
    time_t t;

    // Read RTC date and time from RTC
    if (!ds1302.read(&dt)) {
        Serial.println(F("Error: RTC read failed"));
        delay(3000);
        return;
    }

    // Print Unix Epoch time
    t = ds1302.getEpoch();
    snprintf(buf, sizeof(buf), "%lu", t);
    Serial.print(buf);
    Serial.print(F("  "));

    // Print day of the week, day month, month and year
    strncpy_P(buf, (char *)pgm_read_dword(&(day_week_table[dt.tm_wday])), sizeof(buf));
    Serial.print(buf);
    Serial.print(F(" "));
    Serial.print(dt.tm_mday);
    Serial.print(F(" "));
    strncpy_P(buf, (char *)pgm_read_dword(&(month_table[dt.tm_mon])), sizeof(buf));
    Serial.print(buf);
    Serial.print(F(" "));
    Serial.print(dt.tm_year + 1900);
    Serial.print(F("  "));

    // Print time
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", dt.tm_hour, dt.tm_min, dt.tm_sec);
    Serial.println(buf);

    // Just an example to print the date and time every second. Recommended code is to use the 1Hz
    // square wave out pin with an interrupt instead of polling.
    delay(1000);
}
