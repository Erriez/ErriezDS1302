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
 * \file ErriezDS1302.h
 * \brief DS1302 RTC library for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS1302
 *      Documentation:  https://erriez.github.io/ErriezDS1302
 */

#ifndef ERRIEZ_DS1302_H_
#define ERRIEZ_DS1302_H_

#include <Arduino.h>

//! DS1302 address/command register
#define DS1302_ACB              0x80    //!< Address command date/time
#define DS1302_ACB_RAM          0x40    //!< Address command RAM
#define DS1302_ACB_CLOCK        0x00    //!< Address command clock
#define DS1302_ACB_READ         0x01    //!< Address command read
#define DS1302_ACB_WRITE        0x00    //!< Address command write

//! DS1302 read clock register
#define DS1302_CMD_READ_CLOCK_REG(reg)  (DS1302_ACB | DS1302_ACB_CLOCK | (((reg) & 0x1F) << 1) | DS1302_ACB_READ)
//! DS1302 write clock register
#define DS1302_CMD_WRITE_CLOCK_REG(reg) (DS1302_ACB | DS1302_ACB_CLOCK | (((reg) & 0x1F) << 1) | DS1302_ACB_WRITE)
//! DS1302 read clock register with burst
#define DS1302_CMD_READ_CLOCK_BURST     (DS1302_ACB | DS1302_ACB_CLOCK | 0x3E | DS1302_ACB_READ)
//! DS1302 writeclock register with burst
#define DS1302_CMD_WRITE_CLOCK_BURST    (DS1302_ACB | DS1302_ACB_CLOCK | 0x3E | DS1302_ACB_WRITE)
//! DS1302 read RAM register
#define DS1302_CMD_READ_RAM(addr)       (DS1302_ACB | DS1302_ACB_RAM | (((addr) & 0x1F) << 1) | DS1302_ACB_READ)
//! DS1302 write RAM register
#define DS1302_CMD_WRITE_RAM(addr)      (DS1302_ACB | DS1302_ACB_RAM | (((addr) & 0x1F) << 1) | DS1302_ACB_WRITE)
//! DS1302 read RAM register with burst
#define DS1302_CMD_READ_RAM_BURST       (DS1302_ACB | DS1302_ACB_RAM | 0x3E | DS1302_ACB_READ)
//! DS1302 write RAM register with burst
#define DS1302_CMD_WRITE_RAM_BURST      (DS1302_ACB | DS1302_ACB_RAM | 0x3E | DS1302_ACB_WRITE)

//! DS1302 registers
#define DS1302_REG_SECONDS      0x00    //!< Seconds register
#define DS1302_REG_MINUTES      0x01    //!< Minutes register
#define DS1302_REG_HOURS        0x02    //!< Hours register
#define DS1302_REG_DAY_MONTH    0x03    //!< Day of the month register
#define DS1302_REG_MONTH        0x04    //!< Month register
#define DS1302_REG_DAY_WEEK     0x05    //!< Day of the week register
#define DS1302_REG_YEAR         0x06    //!< Year register
#define DS1302_REG_WP           0x07    //!< Write protect register
#define DS1302_REG_TC           0x08    //!< Tickle Charger register

//! DS1302 number of RAM registers
#define NUM_DS1302_RAM_REGS     31

//! DS1302 register bit defines
#define DS1302_BIT_CH           7       //!< Clock halt bit
#define DS1302_BIT_WP           7       //!< Write protect bit
#define DS1302_BIT_READ         0       //!< Bit read

#define DS1302_TCS_DISABLE      0x5C    //!< Tickle Charger disable value

#ifdef __AVR
#define DS1302_CLK_LOW()        { *portOutputRegister(_clkPort) &= ~_clkBit; }  //!< CLK pin low
#define DS1302_CLK_HIGH()       { *portOutputRegister(_clkPort) |= _clkBit; }   //!< CLK pin high
#define DS1302_CLK_INPUT()      { *portModeRegister(_clkPort) &= ~_clkBit; }    //!< CLK pin input
#define DS1302_CLK_OUTPUT()     { *portModeRegister(_clkPort) |= _clkBit; }     //!< CLK pin output

#define DS1302_IO_LOW()        { *portOutputRegister(_ioPort) &= ~_ioBit; }  //!< IO pin low
#define DS1302_IO_HIGH()       { *portOutputRegister(_ioPort) |= _ioBit; }   //!< IO pin high
#define DS1302_IO_INPUT()      { *portModeRegister(_ioPort) &= ~_ioBit; }    //!< IO pin input
#define DS1302_IO_OUTPUT()     { *portModeRegister(_ioPort) |= _ioBit; }     //!< IO pin output
#define DS1302_IO_READ()       ( *portInputRegister(_ioPort) & _ioBit )      //!< IO pin read

#define DS1302_CE_LOW()        { *portOutputRegister(_cePort) &= ~_ceBit; }  //!< CE pin low
#define DS1302_CE_HIGH()       { *portOutputRegister(_cePort) |= _ceBit; }   //!< CE pin high
#define DS1302_CE_INPUT()      { *portModeRegister(_cePort) &= ~_ceBit; }    //!< CE pin input
#define DS1302_CE_OUTPUT()     { *portModeRegister(_cePort) |= _ceBit; }     //!< CE pin output
#else
#define DS1302_CLK_LOW()        { digitalWrite(_clkPin, LOW); }     //!< CLK pin low
#define DS1302_CLK_HIGH()       { digitalWrite(_clkPin, HIGH); }    //!< CLK pin high
#define DS1302_CLK_INPUT()      { pinMode(_clkPin, INPUT); }        //!< CLK pin input
#define DS1302_CLK_OUTPUT()     { pinMode(_clkPin, OUTPUT); }       //!< CLK pin output

#define DS1302_IO_LOW()        { digitalWrite(_ioPin, LOW); }       //!< IO pin low
#define DS1302_IO_HIGH()       { digitalWrite(_ioPin, HIGH); }      //!< IO pin high
#define DS1302_IO_INPUT()      { pinMode(_ioPin, INPUT); }          //!< IO pin input
#define DS1302_IO_OUTPUT()     { pinMode(_ioPin, OUTPUT); }         //!< IO pin output
#define DS1302_IO_READ()       ( digitalRead(_ioPin) )              //!< IO pin read

#define DS1302_CE_LOW()        { digitalWrite(_cePin, LOW); }       //!< CE pin low
#define DS1302_CE_HIGH()       { digitalWrite(_cePin, HIGH); }      //!< CE pin high
#define DS1302_CE_INPUT()      { pinMode(_cePin, INPUT); }          //!< CE pin input
#define DS1302_CE_OUTPUT()     { pinMode(_cePin, OUTPUT); }         //!< CE pin output
#endif

// Delay defines
#if F_CPU >= 20000000UL
#define DS1302_PIN_DELAY()      { delayMicroseconds(1); }           //!< Delay between pin changes
#else
#define DS1302_PIN_DELAY()                                          //!< Delay between pin changes
#endif

/*!
 * \brief Date time structure
 */
typedef struct {
    uint8_t second;     //!< Second 0..59
    uint8_t minute;     //!< Minute 0..59
    uint8_t hour;       //!< Hour 0..23
    uint8_t dayWeek;    //!< Day of the week (1 = Monday)
    uint8_t dayMonth;   //!< Day of the month 1..31
    uint8_t month;      //!< Month 1..12
    uint16_t year;      //!< Year 2000..2099
} DS1302_DateTime;


//! DS1302 RTC class
class DS1302
{
public:
    explicit DS1302(uint8_t clkPin, uint8_t ioPin, uint8_t cePin);
    virtual bool begin();

    virtual void writeProtect(bool enable);
    virtual bool isWriteProtected();
    virtual void halt(bool halt);
    virtual bool isHalted();
    virtual void setDateTime(DS1302_DateTime *dateTime);
    virtual bool getDateTime(DS1302_DateTime *dateTime);
    virtual void setTime(uint8_t hour, uint8_t minute, uint8_t second);
    virtual bool getTime(uint8_t *hour, uint8_t *minute, uint8_t *second);

    virtual void writeClockRegister(uint8_t reg, uint8_t value);
    virtual uint8_t readClockRegister(uint8_t reg);

    virtual void writeByteRAM(uint8_t addr, uint8_t value);
    virtual void writeBufferRAM(uint8_t *buf, uint8_t len);

    virtual uint8_t readByteRAM(uint8_t addr);
    virtual void readBufferRAM(uint8_t *buf, uint8_t len);

protected:
#ifdef __AVR
    uint8_t _clkPort;   //!< Clock port in IO pin register
    uint8_t _ioPort;    //!< Data port in IO pin register
    uint8_t _cePort;    //!< Chip enable port in IO pin register

    uint8_t _clkBit;    //!< Clock bit number in IO pin register
    uint8_t _ioBit;     //!< Data bit number in IO pin register
    uint8_t _ceBit;     //!< Chip enable bit number in IO pin register
#else
    uint8_t _clkPin;    //!< Clock pin
    uint8_t _ioPin;     //!< Data pin
    uint8_t _cePin;     //!< Chip enable pin
#endif

    // RTC interface functions
    virtual void transferBegin();
    virtual void transferEnd();
    virtual void writeAddrCmd(uint8_t value);
    virtual void writeByte(uint8_t value);
    virtual uint8_t readByte();
    virtual void readBuffer(void *buf, uint8_t len);

    // BCD conversions
    virtual uint8_t bcdToDec(uint8_t bcd);
    virtual uint8_t decToBcd(uint8_t dec);
};

#endif // ERRIEZ_DS1302_H_
