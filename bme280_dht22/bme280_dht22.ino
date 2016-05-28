//based on:
// Adafruit Adafruit_BME280_Library
// https://github.com/adafruit/Adafruit_BME280_Library
//and
// Astuder BMP085-template-library-Energia
// https://github.com/astuder/BMP085-template-library-Energia
//plus code for altitude and relative pressure
//by r7

#include <Wire.h>                                                       // required by BME280 library
#include <BME280_t.h>                                                   // import BME280 template library
#include "DHT.h"

#define ASCII_ESC 27

#define DHTPIN D2
#define DHTTYPE DHT22

// DHT instructions
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// BME instructions
// VIN - 5V
// GND - Ground
// SCL - D4
// SCA - D3

#define MYALTITUDE 80

char bufout[10];

BME280<> BMESensor;                                                     // instantiate sensor
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);                                                 // initialize serial
  Wire.begin(0,2);                                                      // initialize I2C that connects to sensor
  BMESensor.begin();                                                    // initalize bme280 sensor
  dht.begin();
}

void loop() {
  BMESensor.refresh();                                                  // read current sensor data
  sprintf(bufout,"%c[1;0H",ASCII_ESC);
  Serial.print(bufout);

  Serial.println("");
  Serial.println("------------------------------------------------------");   
  Serial.println("------------------------------------------------------");   
  Serial.println("------------------------------------------------------");   
  Serial.println("");
  
  Serial.println("");
  Serial.println("--------------------------BME----------------------------");   
  Serial.println("");

  Serial.print("Temperature: ");
  Serial.print(BMESensor.temperature);                                  // display temperature in Celsius
  Serial.print("C ");

  Serial.print("Humidity: ");
  Serial.print(BMESensor.humidity);                                     // display humidity in %   
  Serial.print("% ");

  Serial.print("Pressure: ");
  Serial.print(BMESensor.pressure  / 100.0F);                           // display pressure in hPa
  Serial.print("hPa ");

  float relativepressure = BMESensor.seaLevelForAltitude(MYALTITUDE);
  Serial.print("RelPress: ");
  Serial.print(relativepressure  / 100.0F);                             // display relative pressure in hPa for given altitude
  Serial.print("hPa ");   

  Serial.print("Altitude: ");
  Serial.print(BMESensor.pressureToAltitude(relativepressure));         // display altitude in m for given pressure
  Serial.print("m");

  Serial.println("");
  Serial.println("--------------------------DHT----------------------------");   
  Serial.println("");
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");


  delay(2000);                                                          // wait a while before next loop
}
