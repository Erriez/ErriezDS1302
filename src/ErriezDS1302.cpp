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

#include "ErriezDS1302.h"

/*!
 * \brief Constructor DS1302 RTC.
 * \param clkPin
 *      Clock pin
 * \param ioPin
 *      I/O pin.
 * \param cePin
 *      Chip select pin. (In previous versions RST pin which is the same)
 */
ErriezDS1302::ErriezDS1302(uint8_t clkPin, uint8_t ioPin, uint8_t cePin)
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
 * \brief Initialize and detect DS1302 RTC.
 * \details
 *      Call this function from setup().
 * \retval true
 *      RTC detected and clock is running.
 * \retval false
 *      RTC not detected.
 */
bool ErriezDS1302::begin()
{
    // Initialize pins
    DS1302_CLK_LOW();
    DS1302_IO_LOW();
    DS1302_CE_LOW();

    DS1302_CLK_OUTPUT();
    DS1302_IO_OUTPUT();
    DS1302_CE_OUTPUT();

    // Check zero bits in day week register
    if (readRegister(DS1302_REG_DAY_WEEK) & 0xF8) {
        return false;
    }

    // Remove write protect
    writeRegister(DS1302_REG_WP, 0);

    // Check write protect bit
    if (readRegister(DS1302_REG_WP) & (1 << DS1302_BIT_WP)) {
        // Error: RTC write protect bit not cleared
        return false;
    }

    return true;
}

/*!
 * \brief Read RTC CH (Clock Halt) from seconds register
 * \retval true
 *      RTC clock is running
 * \retval false
 *      RTC clock is halted
 */
bool ErriezDS1302::isRunning()
{
    // Return status CH bit from seconds register
    if (readRegister(DS1302_REG_SECONDS) & (1 << DS1302_BIT_CH)) {
        return false;
    } else {
        return true;
    }
}

/*!
 * \brief Start RTC clock
 * \details
 *      Clear CH (Clock Halt) bit to seconds register
 */
void ErriezDS1302::clockEnable(bool enable)
{
    uint8_t regSeconds;

    // Read seconds register
    regSeconds = readRegister(DS1302_REG_SECONDS);

    // Set or clear HT bit
    if (enable) {
        // Clear to enable
        regSeconds &= ~(1 << DS1302_BIT_CH);
    } else {
        // Set to disable
        regSeconds |= (1 << DS1302_BIT_CH);
    }

    // Write seconds register
    writeRegister(DS1302_REG_SECONDS, regSeconds);
}

/*!
 * \brief Read Unix UTC epoch time_t
 * \return
 *      Unix epoch time_t seconds since 1970.
 */
time_t ErriezDS1302::getEpoch()
{
    struct tm dt;
    time_t t;

    // Read time structure
    if (!read(&dt)) {
        // RTC read failed
        return 0;
    }

    // Convert date/time struct tm to time_t
    t = mktime(&dt);

    // An offset is needed for AVR target
#ifdef ARDUINO_ARCH_AVR
    t += UNIX_OFFSET;
#endif

    // Return Unix epoch UTC
    return t;
}

/*!
 * \brief Write Unix epoch UTC time to RTC
 * \param t
 *      time_t time
 * \return
 *      See write returns.
 */
void ErriezDS1302::setEpoch(time_t t)
{
    struct tm *dt;

    // Subtract UNIX offset for AVR targets
#ifdef ARDUINO_ARCH_AVR
    t -= UNIX_OFFSET;
#endif

    // Convert time_t to date/time struct tm
    dt = gmtime(&t);

    // Write date/time to RTC
    write(dt);
}

/*!
 * \brief Get RTC date and time
 * \param dt
 *      Date and time structure
 */
bool ErriezDS1302::read(struct tm *dt)
{
    uint8_t buffer[7];

    // Read clock date and time registers
    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_CLOCK_BURST);
    readBuffer(&buffer, sizeof(buffer));
    transferEnd();

    // Clear dt
    memset(dt, 0, sizeof(struct tm));

    // Convert BCD buffer to Decimal
    dt->tm_sec = bcdToDec(buffer[0] & 0x7F);
    dt->tm_min = bcdToDec(buffer[1] & 0x7F);
    dt->tm_hour = bcdToDec(buffer[2] & 0x3f);
    dt->tm_mday = bcdToDec(buffer[3] & 0x3F);
    dt->tm_mon = bcdToDec(buffer[4] & 0x1f);
    dt->tm_wday = bcdToDec(buffer[5] & 0x07);
    dt->tm_year = bcdToDec(buffer[6]) + 100; // 2000-1900

    // Month: 0..11
    if (dt->tm_mon) {
        dt->tm_mon--;
    }

    // Day of the week: 0=Sunday
    if (dt->tm_wday) {
        dt->tm_wday--;
    }

    // Check buffer for valid data
    if ((dt->tm_sec > 59) || (dt->tm_min > 59) || (dt->tm_hour > 23) ||
        (dt->tm_mday < 1) || (dt->tm_mday > 31) || (dt->tm_mon > 11) || (dt->tm_year > 199) ||
        (dt->tm_wday > 6))
    {
        return false;
    }

    return true;
}

/*!
 * \brief Write date and time to RTC.
 * \details
 *      Write all RTC registers at once to prevent a time/date register change in the middle of the
 *      register write operation. This function enables the oscillator.
 * \param dt
 *      Date/time struct tm. Providing invalid date/time data may result in unpredictable behavior.
 */
void ErriezDS1302::write(const struct tm *dt)
{
    // Write clock registers(always 24H)
    transferBegin();
    writeAddrCmd(DS1302_CMD_WRITE_CLOCK_BURST);
    writeByte(decToBcd(dt->tm_sec) & 0x7F); // Remove CH bit
    writeByte(decToBcd(dt->tm_min) & 0x7F);
    writeByte(decToBcd(dt->tm_hour) & 0x3F);
    writeByte(decToBcd(dt->tm_mday) & 0x3F);
    writeByte(decToBcd(dt->tm_mon + 1) & 0x1F);
    writeByte(decToBcd(dt->tm_wday + 1) & 0x07);
    writeByte(decToBcd(dt->tm_year % 100));
    writeByte(0); // Including write protect = 0
    transferEnd();
}

/*!
 * \brief Write time to RTC.
 * \details
 *      Write hour, minute and second registers to RTC.
 * \param hour
 *      Hours 0..23.
 * \param min
 *      Minutes 0..59.
 * \param sec
 *      Seconds 0..59.
 * \retval true
 *      Success.
 * \retval false
 *      Set time failed.
 */
void ErriezDS1302::setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
    struct tm dt;

    read(&dt);
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    write(&dt);
}

/*!
 * \brief Read time from RTC.
 * \details
 *      Read hour, minute and second registers from RTC.
 * \param hour
 *      Hours 0..23.
 * \param min
 *      Minutes 0..59.
 * \param sec
 *      Seconds 0..59.
 * \retval true
 *      Success.
 * \retval false
 *      Invalid second, minute or hour read from RTC. The time is set to zero.
 */
bool ErriezDS1302::getTime(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t buffer[3];

    // Read RTC time registers
    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_CLOCK_BURST);
    readBuffer(&buffer, sizeof(buffer));
    transferEnd();

    // Convert BCD buffer to Decimal
    *sec = bcdToDec(buffer[0] & 0x7f); // Without CH bit from seconds register
    *min = bcdToDec(buffer[1] & 0x7F);
    *hour = bcdToDec(buffer[2] & 0x3F);

    // Check buffer for valid data
    if ((*sec > 59) || (*min > 59) || (*hour > 23)) {
        *sec = 0x00;
        *min = 0x00;
        *hour = 0x00;
        return false;
    }

    return true;
}

/*!
 * \brief Set date time
 * \param hour
 *      Hours 0..23
 * \param min
 *      Minutes 0..59
 * \param sec
 *      Seconds 0..59
 * \param mday
 *      Day of the month 1..31
 * \param mon
 *      Month 1..12 (1=January)
 * \param year
 *      Year 2000..2099
 * \param wday
 *      Day of the week 0..6 (0=Sunday, .. 6=Saturday)
 * \retval true
 *      Success.
 * \retval false
 *      Set time failed.
 */
void ErriezDS1302::setDateTime(uint8_t hour, uint8_t min, uint8_t sec,
                 uint8_t mday, uint8_t mon, uint16_t year,
                 uint8_t wday)
{
    struct tm dt;

    // Prepare struct tm
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    dt.tm_mday = mday;
    dt.tm_mon = mon - 1;
    dt.tm_year = year - 1900;
    dt.tm_wday = wday;

    // Write date/time to RTC
    write(&dt);
}

/*!
 * \brief Write a byte to RAM
 * \param addr
 *      RAM address 0..0x1E
 * \param value
 *      RAM byte 0..0xFF
 */
void ErriezDS1302::writeByteRAM(uint8_t addr, uint8_t value)
{
    transferBegin();
    writeAddrCmd(DS1302_CMD_WRITE_RAM(addr));
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
void ErriezDS1302::writeBufferRAM(uint8_t *buf, uint8_t len)
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
uint8_t ErriezDS1302::readByteRAM(uint8_t addr)
{
    uint8_t value;

    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_RAM(addr));
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
void ErriezDS1302::readBufferRAM(uint8_t *buf, uint8_t len)
{
    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_RAM_BURST);
    for (uint8_t i = 0; i < min((int)len, NUM_DS1302_RAM_REGS); i++) {
        *buf++ = readByte();
    }
    transferEnd();
}

/*!
 * \brief Write clock register
 * \param reg
 *      RTC clock register (See datasheet)
 * \param value
 *      Register value (See datasheet)
 */
void ErriezDS1302::writeRegister(uint8_t reg, uint8_t value)
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
uint8_t ErriezDS1302::readRegister(uint8_t reg)
{
    uint8_t retval;

    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_CLOCK_REG(reg));
    retval = readByte();
    transferEnd();

    return retval;
}

// -------------------------------------------------------------------------------------------------
// Private functions
// -------------------------------------------------------------------------------------------------
/*!
 * \brief BCD to decimal conversion
 * \param bcd
 *      BCD encoded value
 * \return
 *      Decimal value
 */
uint8_t ErriezDS1302::bcdToDec(uint8_t bcd)
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
uint8_t ErriezDS1302::decToBcd(uint8_t dec)
{
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
}

/*!
 * \brief Start RTC transfer
 */
void ErriezDS1302::transferBegin()
{
    DS1302_CLK_LOW();
    DS1302_IO_LOW();
    DS1302_IO_OUTPUT();
    DS1302_CE_HIGH();
}

/*!
 * \brief End RTC transfer
 */
void ErriezDS1302::transferEnd()
{
    DS1302_CE_LOW();
}

/*!
 * \brief Write address/command byte
 * \param value
 *      Address/command byte
 */
void ErriezDS1302::writeAddrCmd(uint8_t value)
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
void ErriezDS1302::writeByte(uint8_t value)
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
uint8_t ErriezDS1302::readByte()
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
void ErriezDS1302::readBuffer(void *buf, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        ((uint8_t *)buf)[i] = readByte();
    }
}