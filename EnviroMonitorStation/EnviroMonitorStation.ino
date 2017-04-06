#include <Arduino.h>

/*
####################################################
# EnviroMonitor.ino
# Part of EnviroMonitor project
# https://github.com/EnviroMonitor
# OpenSource Air Quality Monitoring Solution
# Copyright © 2016 by EnviroMonitor
####################################################
*/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include "Config.h"
#include "SmoglyDHT.h"
#include "PMS3003.h"

#include <Wire.h>
#include <BME280_t.h>

//set debug mode, use only in testing
#define DEBUG_MODE    true
#define TIME_BETWEEN_METERINGS 2000

#define HEAT_PIN_SWITCH D4
#define TARGET_TEMP 30.0

char apiEndpoint[130] = "http://app.smogly.pl/api/v1/metering/";
char token[130] = "";

SmoglyDHT dht;
PMS3003 pms;
BME280<> BME;

void setup() {
  Serial.begin(115200);
  Serial.println();

  Config config;
  config.mount();

  if (DEBUG_MODE) {
    config.reset();
  }

  pinMode(HEAT_PIN_SWITCH, OUTPUT);

  BME.begin();

  config.read("/config.json");

  WiFiManagerParameter custom_apiEndpoint("apiEndpoint", "Smogly URL", apiEndpoint, 130);
  WiFiManagerParameter custom_token("token", "Token", token, 130);

  WiFiManager wifiManager;
  wifiManager.addParameter(&custom_apiEndpoint);
  wifiManager.addParameter(&custom_token);

  if (!wifiManager.autoConnect("Smogly Air Quality Monitor")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  strcpy(apiEndpoint, custom_apiEndpoint.getValue());
  strcpy(token, custom_token.getValue());

  Serial.println("Successfully connected to WiFi network");

  config.save("/config.json");

  Serial.print("Local IP set to: ");
  Serial.println(WiFi.localIP());
  Serial.println("Initializing DHT");
  dht.setup();
  Serial.println("Initializing PMS");
  pms.init();
  delay(TIME_BETWEEN_METERINGS);
}

void loop() {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    pms.read();
    long pm25 = pms.pm25;
    long pm10 = pms.pm10;

    float temp_out1 = BME.humidity;
    float hum_out1 = BME.temperature;
    float bpress_out1 = BME.pressure  / 100.0F;

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    if (t >= TARGET_TEMP) // we're going to measure humidity instead of temp
    {
      digitalWrite(HEAT_PIN_SWITCH, LOW);
      Serial.print(t);
      Serial.print("\tHeater OFF\n");
    } else {
      digitalWrite(HEAT_PIN_SWITCH, HIGH);
      Serial.print(t);
      Serial.print("\tHeater ON\n");
    }

    String output = createPayload(h,t, pm25, pm10, temp_out1, hum_out1, bpress_out1);
    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.POST(output);
    http.end();
    // Wait a few seconds between measurements.
    delay(TIME_BETWEEN_METERINGS);
}

String createPayload(float h, float t, long pm25, long pm10, float temp_out1, float hum_out1, float bpress_out1)
{
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["pm01"] = 0;
  root["pm25"] = pm25;
  root["pm10"] = pm10;
  root["temp_out1"] = t;
  root["temp_out2"] = temp_out1;
  root["temp_out3"] = 0;
  root["temp_int_air1"] = t;
  root["hum_out1"] = hum_out1;
  root["hum_out2"] = 0;
  root["hum_out3"] = 0;
  root["hum_int_air1"] = h;
  root["rssi"] = 0;
  root["bpress_out1"] = bpress_out1;
  root["hw_id"] = "0";
  root["token"] = token;
  String output;
  root.printTo(output);
  return output;
}
