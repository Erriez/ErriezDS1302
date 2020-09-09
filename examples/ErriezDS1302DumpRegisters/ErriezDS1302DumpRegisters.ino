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
 * \brief DS1302 RTC dump registers example for Arduino
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
// power-on / MCU reset / and flashing. This corrupts rtc registers.
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

// Create DS1302 object
ErriezDS1302 rtc = ErriezDS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);


void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1302 dump register example\n"));

    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Enable RTC clock
    if (!rtc.isRunning()) {
        rtc.clockEnable();
    }
}

void loop()
{
    uint8_t buf[DS1302_NUM_CLOCK_REGS + 1];

    // Read all registers
    if (!rtc.readBuffer(0, buf, sizeof(buf))) {
        Serial.println(F("Read buffers failed"));
        return;
    }

    // Print all registers
    Serial.println(F("Registers: "));
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        Serial.print(F("  "));
        Serial.print(i);
        Serial.print(F(": 0x"));
        if (buf[i] < 0x10) {
            Serial.print(F("0"));
        }
        Serial.println(buf[i], HEX);
    }

    delay(1000);
}