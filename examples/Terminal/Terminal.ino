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

/* DS1302 RTC example for Arduino
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


const char* days[] = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
};

const char* months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
};

static unsigned long millisLast = 0;

void setup()
{
    DS1302_DateTime dt;

    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    delay(2000);
    Serial.println(F("DS1302 RTC terminal example\n"));
    Serial.println(F("Press S to set date and time"));

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
}

void loop()
{
    unsigned long currentTime;

    currentTime = millis();
    if (currentTime > millisLast) {
        millisLast = currentTime + 1000;
        rtcPrintDateTime();
    }

    if (Serial.read() == 's') {
        delay(10);
        
        while (Serial.available()) {
            Serial.read();
        }
        Serial.flush();

        // Set RTC date time
        setDateTime();
    }
}

void setDateTime()
{
    DS1302_DateTime dt;

    Serial.println(F("Please enter RTC date and time:"));

    Serial.print(F("Year [00-99]: "));
    dt.year = 2000 + readByte(0, 99);
    Serial.println(dt.year);

    Serial.print(F("Month [1-12]: "));
    dt.month = readByte(1, 12);
    Serial.println(months[dt.month-1]);

    Serial.print(F("Day month [1-31]: "));
    dt.dayMonth = readByte(1, 31);
    Serial.println(dt.dayMonth);

    Serial.print(F("Day of the week [1=Mon-7=Sun]: "));
    dt.dayWeek = readByte(1, 7);
    Serial.println(days[dt.dayWeek-1]);

    Serial.print(F("Hour [0-23]: "));
    dt.hour = readByte(0, 23);
    Serial.println(dt.hour);

    Serial.print(F("Minute [0-59]: "));
    dt.minute = readByte(0, 59);
    Serial.println(dt.minute);

    Serial.print(F("Second [0-59]: "));
    dt.second = readByte(0, 59);
    Serial.println(dt.second);

    // Set RTC date and time
    rtc.setDateTime(&dt);

    millisLast = millis() + 1000;
}

int8_t readByte(int8_t min, int8_t max)
{
    int8_t value = 0;
    int8_t c;

    do {
        while (!Serial.available()) {
            delay(10);
        }

        c = Serial.read();
        while (c != '\n') {
            if (c >= '0' && c <= '9') {
                value = value * 10 + (c - '0');
            }
            c = Serial.read();
        }
        Serial.flush();

        if ((value < min) || (value > max)) {
            Serial.println(F("Wrong input."));
            value = 0;
        }
    } while ((value < min) || (value > max));

    return value;
}

void rtcPrintDateTime()
{
    DS1302_DateTime dt;
    char buf[32];

    // Get RTC date and time
    rtc.getDateTime(&dt);
    snprintf(buf, sizeof(buf), "%d %02d-%02d-%d %d:%02d:%02d",
             dt.dayWeek, dt.dayMonth, dt.month, dt.year, dt.hour, dt.minute, dt.second);
    Serial.println(buf);
}
