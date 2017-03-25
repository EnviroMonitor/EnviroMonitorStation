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
#include <ESP8266WebServer.h>00
#include <WiFiManager.h>

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include "Config.h"
#include "SmoglyDHT.h"
#include "PMS3003.h"

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
#define HEAT_PIN_SWITCH D4

float target_temp = 30.0;

char apiEndpoint[130] = "http://app.smogly.pl/api/v1/station/cef6960f-6627-4c4f-9080-de6bbea41a7e/add-metering/?token=9539f7d1-d633-4838-81cb-6f8c712c6ad0"; //air monitoring API URL, default value

SmoglyDHT dht;
PMS3003 pms;

void setup() {
  Serial.begin(115200);
  Serial.println();

  Config config;
  config.mount();

  if (DEBUG_MODE) {
    config.reset();
  }

  pinMode(HEAT_PIN_SWITCH, OUTPUT);
  digitalWrite(HEAT_PIN_SWITCH, HIGH);

  strcpy(apiEndpoint, config.apiEndpoint);

  config.read("/config.json");

  WiFiManagerParameter custom_apiEndpoint("apiEndpoint", "API endpoint", apiEndpoint, 130);

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
  //strcpy(config.apiEndpoint, custom_apiEndpoint.getValue());

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

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    if (t >= target_temp) // we're going to measure humidity instaead temp
    {
      digitalWrite(HEAT_PIN_SWITCH, LOW);
      Serial.print(t);
      Serial.print("\tHeater OFF\n");
    } else {
      digitalWrite(HEAT_PIN_SWITCH, HIGH);
      Serial.print(t);
      Serial.print("\tHeater ON\n");
    }

    String output = createPayload(h,t, pm25, pm10);
    HTTPClient http;
    http.begin("http://app.smogly.pl/api/v1/station/cef6960f-6627-4c4f-9080-de6bbea41a7e/add-metering/?token=9539f7d1-d633-4838-81cb-6f8c712c6ad0");
    http.addHeader("Content-Type", "application/json");
    http.POST(output);
    http.end();
    // Wait a few seconds between measurements.
    delay(TIME_BETWEEN_METERINGS);
}

String createPayload(float h, float t, long pm25, long pm10)
{
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["pm01"] = "0";
  root["pm25"] = pm25;
  root["pm10"] = pm10;
  root["temp_out1"] = t;
  root["temp_out2"] = "0";
  root["temp_out3"] = "0";
  root["temp_int_air1"] = "0";
  root["hum_out1"] = h;
  root["hum_out2"] = "0";
  root["hum_out3"] = "0";
  root["hum_int_air1"] = "0";
  root["rssi"] = "0";
  root["bpress_out1"] = "0";
  String output;
  root.printTo(output);
  return output;
}
