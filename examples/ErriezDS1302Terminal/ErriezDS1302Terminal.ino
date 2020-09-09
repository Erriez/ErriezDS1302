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
 * \brief DS1302 high precision RTC serial terminal example for Arduino
 * \details
 *      Sources:
 *          https://github.com/Erriez/ErriezDS1302
 *          https://github.com/Erriez/ErriezSerialTerminal
 *      Documentation:
 *          https://erriez.github.io/ErriezDS1302
 *          https://erriez.github.io/ErriezSerialTerminal
 */

#include <ErriezDS1302.h>
#include <ErriezSerialTerminal.h>

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

// Newline character '\r' or '\n'
char newlineChar = '\n';
// Separator character between commands and arguments
char delimiterChar = ' ';

// Create serial terminal object
SerialTerminal term(newlineChar, delimiterChar);

// Define days of the week in flash
const char day_0[] PROGMEM = "Sunday";
const char day_1[] PROGMEM = "Monday";
const char day_2[] PROGMEM = "Tuesday";
const char day_3[] PROGMEM = "Wednesday";
const char day_4[] PROGMEM = "Thursday";
const char day_5[] PROGMEM = "Friday";
const char day_6[] PROGMEM = "Saturday";

const char* const dayWeekTable[] PROGMEM = {
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

const char* const monthTable[] PROGMEM = {
        month_0, month_1, month_2, month_3, month_4, month_5,
        month_6, month_7, month_8, month_9, month_10, month_11
};

bool periodicPrintEnable = false;
bool outputClockEnable = false;


bool getArgumentInt(int *argumentValue)
{
    int val;
    char *arg;

    // Get next decimal or hexadecimal argument string
    arg = term.getNext();
    if (strncmp(arg, "0x", 2) == 0) {
        if (sscanf(arg, "0x%x", &val) != 1) {
            return false;
        }
    } else {
        if (sscanf(arg, "%d", &val) != 1) {
            return false;
        }
    }

    *argumentValue = val;

    return true;
}

void cmdUnknown(const char *command)
{
    // Print unknown command
    Serial.print(F("Unknown command: "));
    Serial.println(command);
}

void cmdHelp()
{
    // Print usage
    Serial.println(F("DS1302 RTC terminal usage:"));
    Serial.println(F("  help or ?          Print this usage"));
    Serial.println();
    Serial.println(F(" Date/time functions:"));
    Serial.println(F("  print              Toggle periodic prints epoch/date/time/temp"));
    Serial.println(F("  dt                 Print date short and time"));
    Serial.println(F("  date               Print date long"));
    Serial.println(F("  time               Print time"));
    Serial.println(F("  epoch              Print epoch"));
    Serial.println(F("  set date <w d-m-Y> Set day week and date"));
    Serial.println(F("  set time <H:M:S>   Set time"));
    Serial.println(F("  set epoch <value>  Set epoch"));
    Serial.println();
    Serial.println(F(" Clock functions:"));
    Serial.println(F("  stop               Stop RTC oscillator"));
    Serial.println(F("  start              Start RTC oscillator"));
    Serial.println();
    Serial.println(F(" Diagnostics:"));
    Serial.println(F("  w [reg] [val]      Write register"));
    Serial.println(F("  r [reg]            Read register"));
}

void cmdTogglePrint()
{
    // Toggle print on/off
    periodicPrintEnable ^= true;

    Serial.print(F("Periodic prints: "));
    Serial.println(periodicPrintEnable ? F("Enabled") : F("Disabled"));
    Serial.println();
}

void cmdPrintDate()
{
    struct tm dt;
    char buf[10];

    Serial.print(F("Date: "));

    // Read date/time from RTC
    if (!rtc.read(&dt)) {
        Serial.println(F("Error: Read date/time failed"));
        return;
    }

    // Print day of the week, day month, month and year
    strncpy_P(buf, (char *)pgm_read_dword(&(dayWeekTable[dt.tm_wday])), sizeof(buf));
    Serial.print(buf);
    Serial.print(F(" "));
    Serial.print(dt.tm_mday);
    Serial.print(F(" "));
    strncpy_P(buf, (char *)pgm_read_dword(&(monthTable[dt.tm_mon])), sizeof(buf));
    Serial.print(buf);
    Serial.print(F(" "));
    Serial.println(dt.tm_year + 1900);
}

void cmdPrintTime()
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    char buf[9];

    Serial.print(F("Time: "));

    // Read time from RTC
    if (!rtc.getTime(&hour, &minute, &second)) {
        Serial.println(F("Error: Read time failed"));
        return;
    }

    // Print time
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", hour, minute, second);
    Serial.println(buf);
}

void cmdPrintDateTime()
{
    struct tm dt;
    char buf[11];

    // Read date/time from RTC
    if (!rtc.read(&dt)) {
        Serial.println(F("Error: Read date/time failed"));
        return;
    }

    Serial.print(F("Date:  "));
    snprintf(buf, sizeof(buf), "%d-%d-%d", dt.tm_mday, dt.tm_mon + 1, dt.tm_year + 1900);
    Serial.println(buf);
    Serial.print(F("Time:  "));
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", dt.tm_hour, dt.tm_min, dt.tm_sec);
    Serial.println(buf);
}

void cmdPrintEpoch()
{
    // Print 32-bit epoch time
    Serial.print(F("Epoch: "));
    Serial.println((uint32_t)rtc.getEpoch());
}

void cmdSetDateTime()
{
    struct tm dt;
    int second;
    int minute;
    int hour;
    int dayWeek;
    int dayMonth;
    int month;
    int year;
    long unsigned int epoch;
    char *arg;

    arg = term.getNext();
    if (strncmp(arg, "date", 4) == 0) {
        arg = term.getRemaining();
        if (arg != NULL) {
            if (sscanf(arg, "%d %d-%d-%d", &dayWeek, &dayMonth, &month, &year) == 4) {
                rtc.read(&dt);
                dt.tm_wday = dayWeek;
                dt.tm_mday = dayMonth;
                dt.tm_mon = month - 1;
                dt.tm_year = year - 1900;
                rtc.write(&dt);
                Serial.print(F("Set date: "));
                Serial.println(arg);
            } else {
                Serial.println(F("Incorrect time format"));
            }
        }
    } else if (strncmp(arg, "time", 4) == 0) {
        arg = term.getRemaining();
        if (arg != NULL) {
            if (sscanf(arg, "%d:%d:%d", &hour, &minute, &second) == 3) {
                rtc.setTime(hour, minute, second);
                Serial.print(F("Set time: "));
                Serial.println(arg);
            } else {
                Serial.println(F("Incorrect time format"));
            }
        }
    } else if (strncmp(arg, "epoch", 5) == 0) {
        arg = term.getRemaining();
        if (arg != NULL) {
            if (sscanf(arg, "%lu", &epoch) == 1) {
                rtc.setEpoch((time_t)epoch);
                Serial.print(F("Set epoch: "));
                Serial.println((uint32_t)epoch);
            } else {
                Serial.println(F("Incorrect time format"));
            }
        }
    } else {
        Serial.println(F("Invalid argument"));
    }
}

void cmdOscillatorStop()
{
    Serial.println(F("Stop oscillator"));

    rtc.clockEnable(false);
}

void cmdOscillatorStart()
{
    Serial.println(F("Start oscillator"));

    rtc.clockEnable(true);
}

void cmdWriteRegister()
{
    int reg;
    int val;
    char buf[13];

    if (getArgumentInt(&reg) != true) {
        Serial.println(F("Error: Incorrect register format"));
        return;
    }

    if (getArgumentInt(&val) != true) {
        Serial.println(F("Error: Incorrect value format"));
        return;
    }

    rtc.writeRegister(reg, val);
    Serial.print(F("Write reg "));
    snprintf(buf, sizeof(buf), "0x%02x: 0x%02x", reg, val);
    Serial.println(buf);
}

void cmdReadRegister()
{
    int reg;
    char buf[13];

    if (getArgumentInt(&reg) != true) {
        Serial.println(F("Error: Incorrect register format"));
        return;
    }

    Serial.print(F("Read reg "));
    snprintf(buf, sizeof(buf), "0x%02x: 0x%02x", reg, rtc.readRegister(reg));
    Serial.println(buf);
}

void printDateTime()
{
    struct tm dt;
    char buf[32];

    // Read date/time from RTC
    if (!rtc.read(&dt)) {
        Serial.println(F("Error: Read date/time failed"));
        return;
    }

    snprintf(buf, sizeof(buf), "%d %02d-%02d-%d %d:%02d:%02d",
             dt.tm_wday,
             dt.tm_mday, dt.tm_mon + 1, dt.tm_year + 1900,
             dt.tm_hour, dt.tm_min, dt.tm_sec);
    Serial.println(buf);
}

void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1302 RTC terminal example\n"));
    Serial.println(F("Type 'help' to display usage."));

    // Set default handler for unknown commands
    term.setDefaultHandler(cmdUnknown);

    // Add command callback handlers
    term.addCommand("?", cmdHelp);
    term.addCommand("help", cmdHelp);

    term.addCommand("print", cmdTogglePrint);
    term.addCommand("dt", cmdPrintDateTime);
    term.addCommand("date", cmdPrintDate);
    term.addCommand("time", cmdPrintTime);
    term.addCommand("epoch", cmdPrintEpoch);
    term.addCommand("set", cmdSetDateTime);

    term.addCommand("stop", cmdOscillatorStop);
    term.addCommand("start", cmdOscillatorStart);

    term.addCommand("w", cmdWriteRegister);
    term.addCommand("r", cmdReadRegister);

    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("DS1302 RTC not found"));
        delay(3000);
    }

    // Check oscillator status
    if (!rtc.isRunning()) {
        Serial.println(F("Warning: RTC clock was stopped."));

        // Enable oscillator
        rtc.clockEnable(true);
    }
}

void loop()
{
    static unsigned long printTimestamp;

    // Read from serial port and handle command callbacks
    term.readSerial();

    // Prints every second
    if (periodicPrintEnable) {
        if ((millis() - printTimestamp) > 1000) {
            printTimestamp = millis();

            // Print epoch/date/time/temperature
            cmdPrintEpoch();
            cmdPrintDateTime();
            Serial.println();
        }
    }
}