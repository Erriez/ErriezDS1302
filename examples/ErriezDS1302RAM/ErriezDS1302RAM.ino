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
 * \brief DS1302 ds1302 RAM example for Arduino
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
// power-on / MCU reset / and flashing. This corrupts ds1302 registers.
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

// Create DS1302 ds1302 object
ErriezDS1302 ds1302 = ErriezDS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);


void fillRAM()
{
    uint8_t buf[NUM_DS1302_RAM_REGS];

    Serial.println(F("Fill DS1302 RAM..."));

    // Fill buffer with some data
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 1 + i;
    }

    // Write buffer to ds1302 RAM
    ds1302.writeBufferRAM(buf, sizeof(buf));
}

void checkRAM()
{
    uint8_t buf[NUM_DS1302_RAM_REGS];

    Serial.print(F("Verify ds1302 RAM: "));

    // Read ds1302 RAM
    ds1302.readBufferRAM(buf, sizeof(buf));

    // Verify contents ds1302 RAM
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        if (buf[i] != 1 + i) {
            // At least one Byte is not correct
            Serial.println(F("FAILED"));
            return;
        }
    }

    // Test passed
    Serial.println(F("Success"));
}

void printRAM()
{
    uint8_t buf[NUM_DS1302_RAM_REGS];

    Serial.print(F("DS1302 RAM: "));

    // Read ds1302 RAM
    ds1302.readBufferRAM(buf, sizeof(buf));

    // Print RAM buffer
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        if (buf[i] < 0x10) {
            Serial.print(F("0"));
        }
        Serial.print(buf[i], HEX);
        Serial.print(F(" "));
    }
    Serial.println();
}

void setup()
{
    uint8_t dataByte;

    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1302 RTC RAM example\n"));

    // Initialize RTC
    while (!ds1302.begin()) {
        Serial.println(F("Error: DS1302 not found"));
        delay(3000);
    }

    // Print DS1302 RAM contents after power-on
    printRAM();

    // Fill entire DS1302 RAM with data
    fillRAM();

    // Verify DS1302 RAM data
    checkRAM();

    // Write byte to RAM
    Serial.println(F("Write value 0xA9 to address 0x02..."));
    ds1302.writeByteRAM(0x02, 0xA9);

    // Read byte from RAM
    Serial.print(F("Read from address 0x02: "));
    dataByte = ds1302.readByteRAM(0x02);

    // Verify written Byte
    if (dataByte == 0xA9) {
        Serial.println(F("Success"));
    } else {
        Serial.println(F("FAILED"));
    }

    // Print ds1302 RAM contents
    printRAM();
}

void loop()
{
}