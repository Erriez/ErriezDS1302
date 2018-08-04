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

/* DS1302 RTC alarm example for Arduino
 *
 * Required library:
 *   https://github.com/Erriez/ErriezDS1302
 */

#include <Arduino.h>


//! Alarm class
class Alarm
{
public:
    /*!
     * \brief Alarm constructor
     * \param hour Alarm hour
     * \param minute Alarm minute
     * \param second Alarm second
     * \param handler Alarm handler
     */
    Alarm(uint8_t hour, uint8_t minute, uint8_t second, void (*handler)()) :
            _hour(hour), _minute(minute), _second(second), _handler(handler)
    {
    }

    /*!
     * \brief Process alarm at every second change
     * \param hour Current hour
     * \param minute Current minute
     * \param second Current second
     */
    void tick(uint8_t hour, uint8_t minute, uint8_t second) {
        if ((hour == _hour) && (minute == _minute) && (second == _second)) {
            (*_handler)();
        }
    }

private:
    uint8_t _hour;        //!< Alarm hour
    uint8_t _minute;      //!< Alarm minute
    uint8_t _second;      //!< Alarm second

    void (*_handler)(); //!< Alarm handler
};
