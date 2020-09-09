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
 * \brief DS1302 RTC set get time example for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS1302
 *    Documentation:  https://erriez.github.io/ErriezDS1302
 */

#include <Wire.h>
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

// Create RTC object
ErriezDS1302 rtc = ErriezDS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);


void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1302 set get time example"));

    // Initialize I2C
    Wire.begin();
    Wire.setClock(100000);

    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Set new time 12:00:00
    if (!rtc.setTime(12, 0, 0)) {
        Serial.println(F("Set time failed"));
    }
}

void loop()
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;

    // Get time from RTC
    if (!rtc.getTime(&hour, &min, &sec)) {
        Serial.println(F("Get time failed"));
    } else {
        // Print time
        Serial.print(hour);
        Serial.print(F(":"));
        if (min < 10) {
            Serial.print(F("0"));
        }
        Serial.print(min);
        Serial.print(F(":"));
        if (sec < 10) {
            Serial.print(F("0"));
        }
        Serial.println(sec);
    }

    // Wait a second
    delay(1000);
}