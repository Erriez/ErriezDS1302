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

/*!
 * \brief DS1302 RTC library for Arduino
 * \details Source: https://github.com/Erriez/ErriezDS1302
 * \file DS1302.cpp
 */

#include "DS1302.h"

/*!
 * \brief Constructor DS1302 RTC.
 * \param clkPin
 *      Clock pin
 * \param ioPin
 *      I/O pin.
 * \param cePin
 *      Chip select pin. (In previous versions RST pin which is the same)
 */
DS1302::DS1302(uint8_t clkPin, uint8_t ioPin, uint8_t cePin)
{
#ifdef __AVR
    _clkPort = digitalPinToPort(clkPin);
    _ioPort = digitalPinToPort(ioPin);
    _cePort = digitalPinToPort(cePin);

    _clkBit = digitalPinToBitMask(clkPin);
    _ioBit = digitalPinToBitMask(ioPin);
    _ceBit = digitalPinToBitMask(cePin);

#else
    _clkPin = clkPin;
    _ioPin = ioPin;
    _cePin = cePin;
#endif
}

/*!
 * \brief Initialize DS1302.
 * \details
 *      Call this function from setup().
 * \return
 *      true:  RTC running
 *      false: RTC halted or not detected
 */
bool DS1302::begin()
{
    // Initialize pins
    DS1302_CLK_LOW();
    DS1302_IO_LOW();
    DS1302_CE_LOW();

    DS1302_CLK_OUTPUT();
    DS1302_IO_OUTPUT();
    DS1302_CE_OUTPUT();

    if (isHalted()) {
        return false;
    } else {
        return true;
    }
}

/*!
 * \brief Set write protect flag
 * \param enable
 *      true:  Enable RTC write protect
 *      false: Disable RTC write protect
 */
void DS1302::writeProtect(bool enable)
{
    writeClockRegister(DS1302_REG_WP, (uint8_t) (enable << 7));
}

/*!
 * \brief Get write protect state
 * \return
 *      true:  RTC registers are read only
 *      false: RTC registers are writable
 */
bool DS1302::isWriteProtected()
{
    if (readClockRegister(DS1302_REG_WP) & (1 << DS1302_BIT_WP)) {
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief Set RTC clock halted or running
 * \param halt
 *      true:  Enable RTC clock
 *      false: Halt RTC clock
 */
void DS1302::halt(bool halt)
{
    uint8_t regOld;
    uint8_t regNew;
    
    regOld = readClockRegister(DS1302_REG_SECONDS);
    if (halt) {
        regNew = (uint8_t)(regOld | (1 << DS1302_BIT_CH));
    } else {
        regNew = (uint8_t)(regOld & ~(1 << DS1302_BIT_CH));
    }
    
    if (regOld != regNew) {
        writeClockRegister(DS1302_REG_SECONDS, regNew);
    }
}

/*!
 * \brief Get RTC halt status
 * \return
 *      true:  RTC clock is halted
 *      false: RTC clock is running
 */
bool DS1302::isHalted()
{
    if (readClockRegister(DS1302_REG_SECONDS) & (1 << DS1302_BIT_CH)) {
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief Set RTC date and time
 * \param dateTime
 *      Date time structure
 */
void DS1302::setDateTime(DS1302_DateTime *dateTime)
{
    uint8_t ch;

    // Read CH bit
    ch = (uint8_t)(readClockRegister(0) & (1 << DS1302_BIT_CH));

    // Write clock registers(always 24H)
    transferBegin();
    writeAddrCmd(DS1302_CMD_WRITE_CLOCK_BURST);
    writeByte(ch | decToBcd((uint8_t)(dateTime->second & 0x7F)));
    writeByte(decToBcd(dateTime->minute));
    writeByte(decToBcd((uint8_t)(dateTime->hour & 0x3F)));
    writeByte(decToBcd((uint8_t)(dateTime->dayMonth & 0x1F)));
    writeByte(decToBcd((uint8_t)(dateTime->month & 0x1F)));
    writeByte(decToBcd((uint8_t)(dateTime->dayWeek & 0x07)));
    writeByte(decToBcd((uint8_t)((dateTime->year - 2000) & 0xFF)));
    writeByte(0); // Including write protect = 0
    transferEnd();
}

/*!
 * \brief Get RTC date and time
 * \param dateTime
 *      Date and time structure
 */
bool DS1302::getDateTime(DS1302_DateTime *dateTime)
{
    uint8_t buf[7];

    // Read clock date and time registers
    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_CLOCK_BURST);
    readBuffer(&buf, sizeof(buf));
    transferEnd();

    // Convert BCD buffer to Decimal
    dateTime->second = bcdToDec(buf[0] & 0x7f); // Without CH bit from seconds register
    dateTime->minute = bcdToDec(buf[1]);
    dateTime->hour = bcdToDec(buf[2]);
    dateTime->dayMonth = bcdToDec(buf[3]);
    dateTime->month = bcdToDec(buf[4]);
    dateTime->dayWeek = bcdToDec(buf[5]);
    dateTime->year = 2000 + bcdToDec(buf[6]);

    // Check buffer for valid data
    if ((dateTime->second > 59) ||
        (dateTime->minute > 59) ||
        (dateTime->hour > 23) ||
        (dateTime->dayMonth < 1) || (dateTime->dayMonth > 31) ||
        (dateTime->month < 1) || (dateTime->month > 12) ||
        (dateTime->dayWeek < 1) || (dateTime->dayWeek > 7) ||
        (dateTime->year > 2099))
    {
        memset(dateTime, 0x00, sizeof(DS1302_DateTime));
        return false;
    }

    return true;
}

/*!
 * \brief Set RTC time
 * \param hour Hours
 * \param minute Minutes
 * \param second Seconds
 */
void DS1302::setTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    DS1302_DateTime dt = { 0 };

    getDateTime(&dt);
    dt.hour = hour;
    dt.minute = minute;
    dt.second = second;
    setDateTime(&dt);
}

/*!
 * \brief Get RTC time
 * \param hour Hours
 * \param minute Minutes
 * \param second Seconds
 */
bool DS1302::getTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    uint8_t buf[3];

    // Read clock time registers
    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_CLOCK_BURST);
    readBuffer(&buf, sizeof(buf));
    transferEnd();

    // Convert BCD buffer to Decimal
    *second = bcdToDec(buf[0] & 0x7f); // Without CH bit from seconds register
    *minute = bcdToDec(buf[1]);
    *hour = bcdToDec(buf[2]);

    // Check buffer for valid data
    if ((*second > 59) || (*minute > 59) || (*hour > 23)) {
        *second = 0x00;
        *minute = 0x00;
        *hour = 0x00;
        return false;
    }

    return true;
}

/*!
 * \brief Write a byte to RAM
 * \param addr
 *      RAM address 0..0x1E
 * \param value
 *      RAM byte 0..0xFF
 */
void DS1302::writeByteRAM(uint8_t addr, uint8_t value)
{
    transferBegin();
    writeAddrCmd((uint8_t)DS1302_CMD_WRITE_RAM(addr));
    writeByte(value);
    transferEnd();
}

/*!
 * \brief Write buffer to RAM address 0x00 (burst write)
 * \param buf
 *      Data buffer
 * \param len
 *      Buffer length 0x01..0x1E
 */
void DS1302::writeBufferRAM(uint8_t *buf, uint8_t len)
{
    transferBegin();
    writeAddrCmd(DS1302_CMD_WRITE_RAM_BURST);
    for (uint8_t i = 0; i < min((int)len, NUM_DS1302_RAM_REGS); i++) {
        writeByte(*buf++);
    }
    transferEnd();
}

/*!
 * \brief Read byte from RAM
 * \param addr
 *      RAM address 0..0x1E
 * \return
 *      RAM byte 0..0xFF
 */
uint8_t DS1302::readByteRAM(uint8_t addr)
{
    uint8_t value;

    transferBegin();
    writeAddrCmd((uint8_t)DS1302_CMD_READ_RAM(addr));
    value = readByte();
    transferEnd();

    return value;
}

/*!
 * \brief Read buffer from RAM address 0x00 (burst read)
 * \param buf
 *      Data buffer
 * \param len
 *      Buffer length
 */
void DS1302::readBufferRAM(uint8_t *buf, uint8_t len)
{
    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_RAM_BURST);
    for (uint8_t i = 0; i < min((int)len, NUM_DS1302_RAM_REGS); i++) {
        *buf++ = readByte();
    }
    transferEnd();
}

// -------------------------------------------------------------------------------------------------
/*!
 * \brief Write clock register
 * \param reg
 *      RTC clock register (See datasheet)
 * \param value
 *      Register value (See datasheet)
 */
void DS1302::writeClockRegister(uint8_t reg, uint8_t value)
{
    transferBegin();
    writeAddrCmd((uint8_t)DS1302_CMD_WRITE_CLOCK_REG(reg));
    writeByte(value);
    transferEnd();
}

/*!
 * \brief Read clock register
 * \param reg
 *      RTC clock register (See datasheet)
 * \return
 *      Register value (See datasheet)
 */
uint8_t DS1302::readClockRegister(uint8_t reg)
{
    uint8_t retval;

    transferBegin();
    writeAddrCmd((uint8_t)DS1302_CMD_READ_CLOCK_REG(reg));
    retval = readByte();
    transferEnd();

    return retval;
}

// -------------------------------------------------------------------------------------------------
// Private functions
// -------------------------------------------------------------------------------------------------
/*!
 * \brief Start RTC transfer
 */
void DS1302::transferBegin()
{
    DS1302_CLK_LOW();
    DS1302_IO_LOW();
    DS1302_IO_OUTPUT();
    DS1302_CE_HIGH();
}

/*!
 * \brief End RTC transfer
 */
void DS1302::transferEnd()
{
    DS1302_CE_LOW();
}

/*!
 * \brief Write address/command byte
 * \param value
 *      Address/command byte
 */
void DS1302::writeAddrCmd(uint8_t value)
{
    // Write 8 bits to RTC
    for (uint8_t i = 0; i < 8; i++) {
        if (value & (1 << i)) {
            DS1302_IO_HIGH();
        } else {
            DS1302_IO_LOW();
        }
        DS1302_PIN_DELAY();
        DS1302_CLK_HIGH();
        DS1302_PIN_DELAY();

        if ((value & (1 << DS1302_BIT_READ)) && (i == 7)) {
            DS1302_IO_INPUT();
        } else {
            DS1302_CLK_LOW();
        }
    }
}

/*!
 * \brief Write byte
 * \param value
 *      Data byte
 */
void DS1302::writeByte(uint8_t value)
{
    // Write 8 bits to RTC
    for (uint8_t i = 0; i < 8; i++) {
        if (value & 0x01) {
            DS1302_IO_HIGH();
        } else {
            DS1302_IO_LOW();
        }
        value >>= 1;
        DS1302_CLK_HIGH();
        DS1302_PIN_DELAY();
        DS1302_CLK_LOW();
    }
}

/*!
 * \brief Read Byte from RTC
 * \return
 *      Data Byte
 */
uint8_t DS1302::readByte()
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; i++) {
        DS1302_CLK_HIGH();
        DS1302_CLK_LOW();
        DS1302_PIN_DELAY();

        value >>= 1;
        if (DS1302_IO_READ()) {
            value |= 0x80;
        } else {
            value &= ~(0x80);
        }
    }

    return value;
}

/*!
 * \brief Read buffer from DS1302
 * \param buf
 *      Buffer
 * \param len
 *      Buffer length
 */
void DS1302::readBuffer(void *buf, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        ((uint8_t *)buf)[i] = readByte();
    }
}

/*!
 * \brief BCD to decimal conversion
 * \param bcd
 *      BCD encoded value
 * \return
 *      Decimal value
 */
uint8_t DS1302::bcdToDec(uint8_t bcd)
{
    return (uint8_t)(10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

/*!
 * \brief Decimal to BCD conversion
 * \param dec
 *      Decimal value
 * \return
 *      BCD encoded value
 */
uint8_t DS1302::decToBcd(uint8_t dec)
{
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
}
