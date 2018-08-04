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

/* DS1302 RTC benchmark for Arduino
 *
 * Required libraries:
 *   https://github.com/Erriez/ErriezDS1302
 *   https://github.com/Erriez/ErriezTimestamp
 */

#include <ErriezDS1302.h>
#include <ErriezTimestamp.h>

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

// Create timestamp with microseconds resolution
TimestampMicros timestamp;


void setup()
{
    DS1302_DateTime dt;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t buf[NUM_DS1302_RAM_REGS] = { 0xFF };

    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS1302 RTC benchmark\n"));

    // Initialize RTC
    Serial.print(F("rtc.begin(): "));
    timestamp.start();
    rtc.begin();
    timestamp.print();

    // Make clock and RAM registers writable
    Serial.print(F("rtc.writeProtect(false): "));
    timestamp.start();
    rtc.writeProtect(false);
    timestamp.print();

    // Enable RTC clock
    Serial.print(F("rtc.halt(false): "));
    timestamp.start();
    rtc.halt(false);
    timestamp.print();

    // Set date time
    dt.hour = 12;
    dt.minute = 0;
    dt.second = 0;
    dt.dayWeek = 1;
    dt.dayMonth = 1;
    dt.month = 1;
    dt.year = 18;
    Serial.print(F("rtc.setDateTime(&dt): "));
    timestamp.start();
    rtc.setDateTime(&dt);
    timestamp.print();

    // Get date time
    Serial.print(F("rtc.getDateTime(&dt): "));
    timestamp.start();
    rtc.getDateTime(&dt);
    timestamp.print();

    // Set time
    Serial.print(F("rtc.setTime(12, 0, 0): "));
    timestamp.start();
    rtc.setTime(12, 0, 0);
    timestamp.print();

    // Get time
    Serial.print(F("rtc.getTime(&hour, &minute, &second): "));
    timestamp.start();
    rtc.getTime(&hour, &minute, &second);
    timestamp.print();

    // Write 1 Byte to RTC RAM
    Serial.print(F("rtc.writeRAM(0x00, 0xFF): "));
    timestamp.start();
    rtc.writeByteRAM(0x00, 0xFF);
    timestamp.print();

    // Write 31 Bytes to RTC RAM
    Serial.print(F("rtc.writeRAM(buf, sizeof(buf): "));
    timestamp.start();
    rtc.writeBufferRAM(buf, sizeof(buf));
    timestamp.print();

    // Read 1 Byte from RTC RAM
    Serial.print(F("rtc.readRAM(0x00): "));
    timestamp.start();
    buf[0] = rtc.readByteRAM(0x00);
    timestamp.print();

    // Read 31 Bytes from RTC RAM
    Serial.print(F("rtc.readRAM(buf, sizeof(buf)): "));
    timestamp.start();
    rtc.readBufferRAM(buf, sizeof(buf));
    timestamp.print();
}

void loop()
{

}
