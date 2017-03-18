#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "Config.h"

void Config::mount()
{
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount file system");
  }
}

void Config::read(const char* fileName) {
  apiEndpoint = "http://app.smogly.pl/api/v1/station/cef6960f-6627-4c4f-9080-de6bbea41a7e/add-metering/?token=9539f7d1-d633-4838-81cb-6f8c712c6ad0";

  if (!SPIFFS.exists(fileName)) {
    Serial.println("Cannot find config file");
    return;
  }

  File configFile = SPIFFS.open(fileName, "r");
  if (!configFile) {
    Serial.println("Cannot open config file");
    return;
  }

  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  if (!json.success()) {
    Serial.println("Failed to load json config");
    return;
  }
  strcpy(apiEndpoint, json["apiEndpoint"]);
}

void Config::save(const char* fileName)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["apiEndpoint"] = apiEndpoint;
  File configFile = SPIFFS.open(fileName, "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  json.printTo(configFile);
  configFile.close();
}

void Config::reset()
{
  Serial.println("*DEBUG MODE ON: formatting SPIFFS");
  SPIFFS.format();
}
