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

/* DHT22 - AM2303 temperature and relative humidity sensor example for Arduino
 *
 * Required library:
 *   https://github.com/Erriez/ErriezDHT22
 */

#include <DHT22.h>

// Connect DTH22 data pin to Arduino DIGITAL pin
#define DHT22_PIN   2

// Create DHT22 sensor object
DHT22 sensor = DHT22(DHT22_PIN);

// Function prototypes
void printTemperature(int16_t temperature);
void printHumidity(int16_t humidity);

void setup()
{
  // Initialize serial port
  Serial.begin(115200);
  Serial.println(F("DHT22 temperature and humidity sensor example\n"));

  // Initialize sensor
  sensor.begin();
}

void loop()
{
  // Check minimum interval of 2000 ms between sensor reads
  if (sensor.available()) {
    // Read temperature from sensor (blocking)
    int16_t temperature = sensor.readTemperature();

    // Read humidity from sensor (blocking)
    int16_t humidity = sensor.readHumidity();

    // Print temperature
    printTemperature(temperature);

    // Print humidity
    printHumidity(humidity);
  }
}

void printTemperature(int16_t temperature)
{
  // Check valid temperature value
  if (temperature == ~0) {
    // Temperature error (Check hardware connection)
    Serial.println(F("Temperature: Error"));
  } else {
    // Print temperature
    Serial.print(F("Temperature: "));
    Serial.print(temperature / 10);
    Serial.print(F("."));
    Serial.print(temperature % 10);

    // Print degree Celsius symbols
    // Choose if (1) for normal or if (0) for extended ASCII degree symbol
    if (1) {
      // Print *C characters which are displayed correctly in the serial
      // terminal of the Arduino IDE
      Serial.println(F(" *C"));
    } else {
      // Note: Extended characters are not supported in the Arduino IDE and
      // displays ?C. This is displayed correctly with other serial terminals
      // such as Tera Term.
      // Degree symbol is ASCII code 248 (decimal).
      char buf[4];
      snprintf_P(buf, sizeof(buf), PSTR(" %cC"), 248);
      Serial.println(buf);
    }
  }
}

void printHumidity(int16_t humidity)
{
  // Check valid humidity value
  if (humidity == ~0) {
    // Humidity error (Check hardware connection)
    Serial.println(F("Humidity: Error"));
  } else {
    // Print humidity
    Serial.print(F("Humidity: "));
    Serial.print(humidity / 10);
    Serial.print(F("."));
    Serial.print(humidity % 10);
    Serial.println(F(" %"));
  }

  Serial.println();
}
