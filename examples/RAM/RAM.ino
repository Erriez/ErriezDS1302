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

/* DS1302 RTC RAM example for Arduino
 *
 * Required library:
 *   https://github.com/Erriez/ErriezDS1302
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
DS1302 rtc = DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);


// Function prototypes
void fillRAM();
void printRAM();


void setup()
{
    uint8_t dataByte;

    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS1302 RTC RAM example\n"));

    // Initialize RTC
    rtc.begin();

    // Make clock and RAM registers writable
    rtc.writeProtect(false);

    // Check RTC protect state
    if (rtc.isWriteProtected()) {
        Serial.println(F("Error: DS1302 not found"));
        while (1) {
            ;
        }
    }

    // Print RTC RAM contents after power-on
    printRAM();

    // Fill entire RTC RAM with data
    fillRAM();

    // Verify RTC RAM data
    checkRAM();

    // Write byte to RAM
    Serial.println(F("Write value 0xA9 to address 0x02..."));
    rtc.writeByteRAM(0x02, 0xA9);

    // Read byte from RAM
    Serial.print(F("Read from address 0x02: "));
    dataByte = rtc.readByteRAM(0x02);

    // Verify written Byte
    if (dataByte == 0xA9) {
        Serial.println(F("Success"));
    } else {
        Serial.println(F("FAILED"));
    }

    // Print RTC RAM contents
    printRAM();
}

void loop()
{
    printRAM();
    delay(1000);
}

void fillRAM()
{
    uint8_t buf[NUM_DS1302_RAM_REGS];

    Serial.println(F("Fill RTC RAM..."));

    // Fill buffer with some data
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 1 + i;
    }

    // Write buffer to RTC RAM
    rtc.writeBufferRAM(buf, sizeof(buf));
}

void checkRAM()
{
    uint8_t buf[NUM_DS1302_RAM_REGS];

    Serial.print(F("Verify RTC RAM: "));

    // Read RTC RAM
    rtc.readBufferRAM(buf, sizeof(buf));

    // Verify contents RTC RAM
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

    Serial.print(F("RTC RAM: "));

    // Read RTC RAM
    rtc.readBufferRAM(buf, sizeof(buf));

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
