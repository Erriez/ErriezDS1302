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
 * \brief DS1302 RTC test example for Arduino
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
ErriezDS1302 ds1302 = ErriezDS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

#define Success     true
#define Failure     false

#define CHK(err) {                      \
    if ((err) == Success) {             \
        while (0)                       \
            ;                           \
    } else {                            \
        Serial.print(F("Failure: "));     \
        Serial.print((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__));     \
        Serial.print(F(":"));           \
        Serial.println(__LINE__);       \
        Serial.flush();                 \
        noInterrupts();                 \
        while (1)                       \
            ;                           \
    }                                   \
}

// A test epoch "Sunday, September 6, 2020 18:20:30"
#define EPOCH_TEST      1599416430UL


void setup()
{
    struct tm dtw;
    struct tm dtr;
    uint8_t hour, min, sec;

    // Initialize serial port
    Serial.begin(115200);
    delay(500);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1302 begin test"));

    // Initialize RTC
    while (!ds1302.begin()) {
        Serial.println(F("Error: DS1302 not found"));
        delay(3000);
    }

    // Test oscillator
    if (ds1302.isRunning()) {
        ds1302.clockEnable(true);
    }
    CHK(ds1302.isRunning() == true);

    // Test epoch write
    ds1302.setEpoch(EPOCH_TEST);
    delay(1500);

    // Verify epoch struct tm
    CHK(ds1302.read(&dtr) == Success);
    CHK(dtr.tm_sec == 31);
    CHK(dtr.tm_min == 20);
    CHK(dtr.tm_hour == 18);
    CHK(dtr.tm_mday == 6);
    CHK(dtr.tm_mon == 8);
    CHK(dtr.tm_year == 120);
    CHK(dtr.tm_wday == 0);

    // Verify epoch time_t
    CHK(ds1302.getEpoch() == (EPOCH_TEST + 1));

    // Test write
    dtw.tm_hour = 12;
    dtw.tm_min = 34;
    dtw.tm_sec = 56;
    dtw.tm_mday = 29;
    dtw.tm_mon = 1; // 0=January
    dtw.tm_year = 2020-1900;
    dtw.tm_wday = 6; // 0=Sunday
    ds1302.write(&dtw);

    // Test read
    CHK(ds1302.read(&dtr) == Success);
    CHK(dtw.tm_sec == dtr.tm_sec);
    CHK(dtw.tm_min == dtr.tm_min);
    CHK(dtw.tm_hour == dtr.tm_hour);
    CHK(dtw.tm_mday == dtr.tm_mday);
    CHK(dtw.tm_mon == dtr.tm_mon);
    CHK(dtw.tm_year == dtr.tm_year);
    CHK(dtw.tm_wday == dtr.tm_wday);

    // Test set time
    ds1302.setTime(12, 34, 56);
    CHK(ds1302.getTime(&hour, &min, &sec) == Success);
    CHK((hour == 12) && (min == 34) && (sec == 56));

    // Test set date/time   13:45:09  31 December 2019  Tuesday
    ds1302.setDateTime(13, 45, 9,  31, 12, 2019,  2);

    // Verify setDateTime
    CHK(ds1302.read(&dtr) == Success);
    CHK(dtr.tm_sec == 9);
    CHK(dtr.tm_min == 45);
    CHK(dtr.tm_hour == 13);
    CHK(dtr.tm_mday == 31);
    CHK(dtr.tm_mon == 11);    // Month 0..11
    CHK(dtr.tm_year == (2019 - 1900)); // Year - 1900
    CHK(dtr.tm_wday == 2);    // 0=Sun, 1=Mon, 2=Tue

    // Completed
    Serial.println(F("Test passed"));
}

void loop()
{
}