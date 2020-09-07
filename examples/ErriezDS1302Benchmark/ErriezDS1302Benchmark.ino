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
 * \brief DS1302 ds1302 benchmark for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS1302
 *    Documentation:  https://erriez.github.io/ErriezDS1302
 */

#include <ErriezDS1302.h>
#include <ErriezTimestamp.h> // https://github.com/Erriez/ErriezTimestamp

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

// Create DS1302 object
ErriezDS1302 ds1302 = ErriezDS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

// Create timestamp with microseconds resolution
TimestampMicros timestamp;


void setup()
{
    struct tm dt;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t buf[NUM_DS1302_RAM_REGS] = { 0xFF };
    time_t t;

    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1302 RTC benchmark\n"));

    // Initialize ds1302
    Serial.print(F("ds1302.begin(): "));
    timestamp.start();
    ds1302.begin();
    timestamp.print();

    // Set date time
    dt.tm_hour = 12;
    dt.tm_min = 0;
    dt.tm_sec = 0;
    dt.tm_wday = 1; // 0=Sunday
    dt.tm_mday = 1;
    dt.tm_mon = 1;  // Month 0..11
    dt.tm_year = 2020-1900;
    Serial.print(F("ds1302.setDateTime(&dt): "));
    timestamp.start();
    ds1302.write(&dt);
    timestamp.print();

    // Read struct tm
    Serial.print(F("ds1302.read(&dt): "));
    timestamp.start();
    ds1302.read(&dt);
    timestamp.print();

    // Read epoch
    Serial.print(F("ds1302.getEpoch(&dt): "));
    timestamp.start();
    t = ds1302.getEpoch();
    timestamp.print();

    // Write epoch
    Serial.print(F("ds1302.setEpoch(&dt): "));
    timestamp.start();
    ds1302.setEpoch(t);
    timestamp.print();

    // Set time
    Serial.print(F("ds1302.setTime(12, 0, 0): "));
    timestamp.start();
    ds1302.setTime(12, 0, 0);
    timestamp.print();

    // Get time
    Serial.print(F("ds1302.getTime(&hour, &minute, &second): "));
    timestamp.start();
    ds1302.getTime(&hour, &minute, &second);
    timestamp.print();

    // Write 1 Byte to ds1302 RAM
    Serial.print(F("ds1302.writeRAM(0x00, 0xFF): "));
    timestamp.start();
    ds1302.writeByteRAM(0x00, 0xFF);
    timestamp.print();

    // Write 31 Bytes to ds1302 RAM
    Serial.print(F("ds1302.writeRAM(buf, sizeof(buf): "));
    timestamp.start();
    ds1302.writeBufferRAM(buf, sizeof(buf));
    timestamp.print();

    // Read 1 Byte from ds1302 RAM
    Serial.print(F("ds1302.readRAM(0x00): "));
    timestamp.start();
    buf[0] = ds1302.readByteRAM(0x00);
    timestamp.print();

    // Read 31 Bytes from ds1302 RAM
    Serial.print(F("ds1302.readRAM(buf, sizeof(buf)): "));
    timestamp.start();
    ds1302.readBufferRAM(buf, sizeof(buf));
    timestamp.print();
}

void loop()
{
}