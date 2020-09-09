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
 * \file ErriezDS1302.cpp
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
 *      RTC detected.
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

    // DS1302 detected
    return true;
}

/*!
 * \brief Read RTC CH (Clock Halt) from seconds register.
 * \details
 *      The application is responsible for checking the CH (Clock Halt) bit before reading
 *      date/time date. This function may be used to judge the validity of the date/time registers.
 * \retval true
 *      RTC clock is running.
 * \retval false
 *      The date/time data is invalid when the CH bit is set. The application should enable the
 *      oscillator, or program a new date/time.
 */
bool ErriezDS1302::isRunning()
{
    // Return status CH (Clock Halt) bit from seconds register
    if (readRegister(DS1302_REG_SECONDS) & (1 << DS1302_SEC_CH)) {
        // RTC clock stopped
        return false;
    } else {
        // RTC clock is running
        return true;
    }
}

/*!
 * \brief Enable or disable oscillator.
 * \details
 *      Clear or set CH (Clock Halt) bit to seconds register
 * \param enable
 *      true:  Enable RTC clock.\n
 *      false: Stop RTC clock.
 * \retval true
 *      Success.
 * \retval false
 *      Oscillator enable failed.
 */
bool ErriezDS1302::clockEnable(bool enable)
{
    uint8_t regSeconds;

    // Read seconds register
    regSeconds = readRegister(DS1302_REG_SECONDS);

    // Set or clear HT bit
    if (enable) {
        // Clear to enable
        regSeconds &= ~(1 << DS1302_SEC_CH);
    } else {
        // Set to disable
        regSeconds |= (1 << DS1302_SEC_CH);
    }

    // Write seconds register
    return writeRegister(DS1302_REG_SECONDS, regSeconds);
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
 * \retval true
 *      Success.
 * \retval false
 *      Set epoch failed.
 */
bool ErriezDS1302::setEpoch(time_t t)
{
    struct tm *dt;

    // Subtract UNIX offset for AVR targets
#ifdef ARDUINO_ARCH_AVR
    t -= UNIX_OFFSET;
#endif

    // Convert time_t to date/time struct tm
    dt = gmtime(&t);

    // Write date/time to RTC
    return write(dt);
}

/*!
 * \brief Read date and time from RTC.
 * \details
 *      Read all RTC registers at once to prevent a time/date register change in the middle of the
 *      register read operation.
 * \param dt
 *      Date and time struct tm.
 * \retval true
 *      Success
 * \retval false
 *      Read failed.
 */
bool ErriezDS1302::read(struct tm *dt)
{
    uint8_t buffer[DS1302_NUM_CLOCK_REGS];

    // Read clock date and time registers
    if (!readBuffer(0x00, buffer, sizeof(buffer))) {
        memset(dt, 0, sizeof(struct tm));
        return false;
    }

    // Clear dt
    memset(dt, 0, sizeof(struct tm));

    // Convert BCD buffer to Decimal
    dt->tm_sec = bcdToDec(buffer[0] & 0x7F);
    dt->tm_min = bcdToDec(buffer[1] & 0x7F);
    dt->tm_hour = bcdToDec(buffer[2] & 0x3F);
    dt->tm_mday = bcdToDec(buffer[3] & 0x3F);
    dt->tm_mon = bcdToDec(buffer[4] & 0x1F);
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
 * \retval true
 *      Success.
 * \retval false
 *      Write failed.
 */
bool ErriezDS1302::write(const struct tm *dt)
{
    // Buffer including write protect = 0
    uint8_t buffer[DS1302_NUM_CLOCK_REGS + 1];

    // Encode date time from decimal to BCD
    buffer[0] = decToBcd(dt->tm_sec) & 0x7F; // Clear CH bit in seconds register
    buffer[1] = decToBcd(dt->tm_min) & 0x7F;
    buffer[2] = decToBcd(dt->tm_hour) & 0x3F;
    buffer[3] = decToBcd(dt->tm_mday) & 0x3F;
    buffer[4] = decToBcd(dt->tm_mon + 1) & 0x1F;
    buffer[5] = decToBcd(dt->tm_wday + 1) & 0x07;
    buffer[6] = decToBcd(dt->tm_year % 100);
    buffer[7] = 0; // Write protect register

    // Write BCD encoded buffer to RTC registers
    return writeBuffer(0x00, buffer, sizeof(buffer));
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
bool ErriezDS1302::setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
    struct tm dt;

    // Read date/time from RTC
    read(&dt);

    // Update time
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;

    // Write date/time to RTC
    return write(&dt);
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
    struct tm dt;

    // Read date/time from RTC
    if (!read(&dt)) {
        return false;
    }

    // Set return values
    *hour = dt.tm_hour;
    *min = dt.tm_min;
    *sec = dt.tm_sec;

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
 *      Set date/time failed.
 */
bool ErriezDS1302::setDateTime(uint8_t hour, uint8_t min, uint8_t sec,
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
    return write(&dt);
}

/*!
 * \brief Get date time
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
 *      Get date/time failed.
 */
bool ErriezDS1302::getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec,
                               uint8_t *mday, uint8_t *mon, uint16_t *year,
                               uint8_t *wday)
{
    struct tm dt;

    // Read date/time from RTC
    if (!read(&dt)) {
        return false;
    }

    // Set return values
    *hour = dt.tm_hour;
    *min = dt.tm_min;
    *sec = dt.tm_sec;
    *mday = dt.tm_mday;
    *mon = dt.tm_mon + 1;
    *year = dt.tm_year + 1900;
    *wday = dt.tm_wday;

    return true;
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
    for (uint8_t i = 0; i < min((int)len, DS1302_NUM_RAM_REGS); i++) {
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
    for (uint8_t i = 0; i < min((int)len, DS1302_NUM_RAM_REGS); i++) {
        *buf++ = readByte();
    }
    transferEnd();
}

/*!
 * \brief BCD to decimal conversion.
 * \param bcd
 *      BCD encoded value.
 * \return
 *      Decimal value.
 */
uint8_t ErriezDS1302::bcdToDec(uint8_t bcd)
{
    return (uint8_t)(10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

/*!
 * \brief Decimal to BCD conversion.
 * \param dec
 *      Decimal value.
 * \return
 *      BCD encoded value.
 */
uint8_t ErriezDS1302::decToBcd(uint8_t dec)
{
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
}

/*!
 * \brief Read register.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x09.
 * \returns value
 *      8-bit unsigned register value.
 */
uint8_t ErriezDS1302::readRegister(uint8_t reg)
{
    uint8_t value = 0;

    // Read 8-bit unsigned value from clock register
    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_CLOCK_REG(reg));
    value = readByte();
    transferEnd();

    return value;
}

/*!
 * \brief Write register.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x09.
 * \param value
 *      8-bit unsigned register value.
 * \retval true
 *      Success
 * \retval false
 *      Write register failed
 */
bool ErriezDS1302::writeRegister(uint8_t reg, uint8_t value)
{
    // Write 8-bit unsigned value to clock register
    transferBegin();
    writeAddrCmd((uint8_t)DS1302_CMD_WRITE_CLOCK_REG(reg));
    writeByte(value);
    transferEnd();

    return true;
}

/*!
 * \brief Write buffer to RTC clock registers.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x09.
 * \param buffer
 *      Buffer.
 * \param writeLen
 *      Buffer length. Writing is only allowed within valid RTC registers.
 * \retval true
 *      Success
 * \retval false
 *      Write failed.
 */
bool ErriezDS1302::writeBuffer(uint8_t reg, void *buffer, uint8_t writeLen)
{
    if ((reg != 0) || (writeLen != (DS1302_NUM_CLOCK_REGS + 1))) {
        // Burst command requires all clock registers including write protect
        return false;
    }

    // Write buffer with clock burst command to clock registers
    transferBegin();
    writeAddrCmd(DS1302_CMD_WRITE_CLOCK_BURST);
    for (uint8_t i = 0; i < min((int)writeLen, DS1302_NUM_RAM_REGS); i++) {
        writeByte(((uint8_t *)buffer)[i]);
    }
    transferEnd();

    return true;
}

/*!
 * \brief Read buffer from RTC clock registers.
 * \param reg
 *      RTC register number 0x00..0x07.
 * \param buffer
 *      Buffer.
 * \param readLen
 *      Buffer length. Reading is only allowed within valid RTC registers.
 * \retval true
 *      Success
 * \retval false
 *      Read failed.
 */
bool ErriezDS1302::readBuffer(uint8_t reg, void *buffer, uint8_t readLen)
{
    if (reg != 0) {
        // Burst command requires address 0
        return false;
    }

    // Read buffer with clock burst command from clock registers
    transferBegin();
    writeAddrCmd(DS1302_CMD_READ_CLOCK_BURST);
    for (uint8_t i = 0; i < min((int)readLen, DS1302_NUM_RAM_REGS); i++) {
        ((uint8_t *)buffer)[i] = readByte();
    }
    transferEnd();

    return true;
}

// -------------------------------------------------------------------------------------------------
// Private functions
// -------------------------------------------------------------------------------------------------
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