####################################################
# EnviroMonitor.ino
# Part of EnviroMonitor project
# https://github.com/EnviroMonitor
# OpenSource Air Quality Monitoring Solution
# Copyright © 2016 by EnviroMonitor
####################################################

#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>

#define DEBUG_MODE    true

#define HW  0001
#define SW  0001

# Wemos PIN definitions
#define PMS_SET
#define PMS_RST
#define PMS_RXD
#define PMS_TXD
#define PMS_SET
#define PMS_SET


###############################################
# start WiFi AutoConfiguration

//define your default values here, if there are different values in config.json, they are overwritten.
//char station_id[40]; //monitoring device ID, used for identification
char api_endpoint[129] = "http://air-monitor.org/api/v1/"; //air monitoring API URL, default value
char ota_server[129] = "http://air-monitor.org/ota/v1/"; //OTA update server address

//flag for saving data
bool shouldSaveConfig = true;

//set debug mode, use only in testing
bool debugMode = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Config should be saved");
  shouldSaveConfig = true;
}

# end WiFi AutoConfiguration 

###############################################
# begin OTA updates

# end OTA updates

###############################################
# begin PMS3003

# end PMS3003

###############################################
# begin BME280

# end BMWE280

###############################################
# begin DHT22

# end DHT22

###############################################
# start Si7021

# end Si7021

###############################################
# begin DS18B29

# end DS18B29

void setup() {
  Serial.begin(115200);
  Serial.println();

  //clean FS in debug mode
  if debugMode {
    SPIFFS.format();
  }

  //read configuration from filesystem
  Serial.println("mounting file system...");

  if (SPIFFS.begin()) {
    Serial.println("file system mounted");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("config file opened");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(device_id, json["station_id"]);
          strcpy(api_endpoint, json["api_endpoint"]);
          strcpy(ota_server, jsno["ota_server"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //read configuration end



  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_station_id("station_id", "Monitoring station ID", station_id, 40);
  WiFiManagerParameter custom_api_endpoint("api_endpoint", "API endpoint", api_endpoint, 129);
  WiFiManagerParameter custom_ota_server("ota_server", "OTA update server address", ota_server, 129);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  //add all your parameters here
  wifiManager.addParameter(&custom_device_id);
  wifiManager.addParameter(&custom_api_endpoint);
  wifiManager.addParameter(&custom_ota_server);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("Successfully connected to WiFi network");

  //read updated parameters
  strcpy(station_id, custom_station_id.getValue());
  strcpy(api_endpoint, custom_api_endpoint.getValue());
  strcpy(ota_server, custom_ota_server.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["station_id"] = station_id;
    json["api_endpoint"] = api_endpoint;
    json["ota_server"] = ota_server;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

}

void loop() {
  // put your main code here, to run repeatedly:


}
