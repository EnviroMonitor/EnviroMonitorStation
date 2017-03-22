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

//set debug mode, use only in testing
#define DEBUG_MODE    true
#define TIME_BETWEEN_METERINGS 2000

//# Wemos PIN definitions
#define PMS_SET
#define PMS_RST
#define PMS_RXD
#define PMS_TXD
#define PMS_SET
#define PMS_SET

char apiEndpoint[200] = "http://app.smogly.pl/api/v1/station/cef6960f-6627-4c4f-9080-de6bbea41a7e/add-metering/?token=9539f7d1-d633-4838-81cb-6f8c712c6ad0"; //air monitoring API URL, default value

SmoglyDHT dht;

//###############################################
//# begin PMS3003

//# end PMS3003

//###############################################
//# begin BME280

//# end BMWE280

//###############################################
//# begin DHT22

//# end DHT22

//###############################################
//# start Si7021

//# end Si7021

//###############################################
//# begin DS18B29

//# end DS18B29

void setup() {
  Serial.begin(115200);
  Serial.println();

  Config config;

  config.mount();

  if (DEBUG_MODE) {
    config.reset();
  }

  strcpy(apiEndpoint, config.apiEndpoint);

  config.read("/config.json");

  WiFiManagerParameter custom_apiEndpoint("apiEndpoint", "API endpoint", config.apiEndpoint, 200);

  WiFiManager wifiManager;
  wifiManager.addParameter(&custom_apiEndpoint);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("Smogly Air Quality Monitor")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("Successfully connected to WiFi network");

  //read updated parameters
  strcpy(config.apiEndpoint, custom_apiEndpoint.getValue());

  config.save("/config.json");

  Serial.print("Local IP set to: ");
  Serial.println(WiFi.localIP());
  Serial.println("Initializing DHT");
  dht.setup();
}

void loop() {
    // Wait a few seconds between measurements.
    delay(TIME_BETWEEN_METERINGS);

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

    String output = createPayload(h,t);
    HTTPClient http;
    http.begin(apiEndpoint);
    Serial.print(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.POST(output);
    Serial.print(output);
    http.end();

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C\n");
}

String createPayload(float h, float t)
{
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["pm01"] = "10";
  root["pm25"] = "5";
  root["pm10"] = "3";
  root["temp_out1"] = t;
  root["temp_out2"] = "11";
  root["temp_out3"] = "12";
  root["temp_int_air1"] = "12";
  root["hum_out1"] = h;
  root["hum_out2"] = "22";
  root["hum_out3"] = "23";
  root["hum_int_air1"] = "23";
  root["rssi"] = "123";
  root["bpress_out1"] = "24";
  String output;
  root.printTo(output);
  return output;
}
