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

//set debug mode, use only in testing
#define DEBUG_MODE    true

//# Wemos PIN definitions
#define PMS_SET
#define PMS_RST
#define PMS_RXD
#define PMS_TXD
#define PMS_SET
#define PMS_SET

char apiEndpoint[129] = "http://air-monitor.org/api/v1/"; //air monitoring API URL, default value

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

  config.read("/config.json");
  strcpy(apiEndpoint, config.apiEndpoint);

  WiFiManagerParameter custom_apiEndpoint("apiEndpoint", "API endpoint", apiEndpoint, 129);

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
}

void loop() {
    String output = createPayload();
    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.POST(output);
    http.end();
    delay(4000);
}

String createPayload()
{
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["pm01"] = "10";
  root["pm25"] = "5";
  root["pm10"] = "3";
  root["temp_out1"] = "10";
  root["temp_out2"] = "11";
  root["temp_out3"] = "12";
  root["temp_int_air1"] = "12";
  root["hum_out1"] = "21";
  root["hum_out2"] = "22";
  root["hum_out3"] = "23";
  root["hum_int_air1"] = "23";
  root["rssi"] = "123";
  root["bpress_out1"] = "24";
  String output;
  root.printTo(output);
  Serial.println(output);
  return output;
}
