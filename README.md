# EnviroMonitor
ESP8266 based environmental monitoring station. Includes temperature, humidity, barometric pressure and PM2.5, PM10 dust monitoring.

Features:
- PM2.5, PM10 monitoring
- temperature, humidity, barometric pressure monitoring
- heater for exsiccating incoming air
- post data to custom frontend
- post data to Wunderground
- OTA updates for software
- WiFi auto configuration
- 12V/5V powered

We also designed [PCB](https://github.com/EnviroMonitor/EnviroMonitorElectronics) and [enclosure](https://github.com/EnviroMonitor/EnviroMonitorEnclosure) so you can easily build your own sensor. Moreover - every piece of the project is Open Source, so you can modify it for yout needs.

Please stay tuned, this is work in progress. Prototypes are being tested and we will post usable software and HW design soon.

# Hardware

## Bill of Materials
- Wemos D1 mini ESP8266 based development board (example: https://goo.gl/nk0Xvn)
- PMS3003 - Plantower particulate matter sensor detecting PM1.0, PM2.5, PM10 (example: https://goo.gl/ZB6P51)
- BME280 - multisensor (example: https://goo.gl/bCfaJp)
- Si7021 temp/humidity sensor (example: https://goo.gl/RCU0Vk)
- DHT22 temp/humidity sensor (example: https://goo.gl/guQqyB)
- 12V or 5V heat plate (example: https://goo.gl/Pexr5R)
- DC-DC step down power supply module, preferably based on LM2596 (example: https://goo.gl/TbNs1Y)
- AC power supply with 12V DC 1A output (example: https://goo.gl/fVu5LT)
- [PCB](https://github.com/EnviroMonitor/EnviroMonitorElectronics)
- [enclosure](https://github.com/EnviroMonitor/EnviroMonitorEnclosure)

# API

TODO

# Software
## Backend configuration
Before sensor is added to the system, it needs to be registered on the backend side. After registeration you will receive couple of paramaters:
- sensor ID - used to identify the sensor
- API key - to be able to post and receive data from backend
## Sensor configuration
After sensor is connected to power, it start local AccessPoint for initiall configuration. User needs to connect to this AccessPoint and as a next step, familiar HotSpot configuration page should be presented. Using this simple page user can configure:
- sensor ID (generated during sensor registeraion)
- API access key (generated during sensor registeraion)
- WiFi network and password to use for sending data
- backend and OTA server address

Once you finish configuration, sensor will reboot, and join configured WiFi network. If there was any error , e.g. wrong password, sensor will again reboot into AccessPoint mode, so you are able to correct configuration.

## How we measure paramaters
In every cycle device measures couple of paramaters and sends them to EnviroMonitorWe backend. In current version of harware we measure:
- PM1.0, PM2.5 and PM10
- external temperature and humidity
- external barometric pressure
- temperature and humidity of air incoming to PMS3003 sensor

Before we start measuring PM* paramaters, we ensure that incoming air is of accepted paramaters: it's humidity and temperature are acceptable. This is required because humid air particles have size similar to PM1 and PM2.5 and can impact precision of measurment. To overcome this effect one could try to figure out how humidity and temperature impacts the measurement (develop mathematical function) or heat the air. We decided to incorporate heating plate near PMS3003 air intake. 

Once the air reaches accepted humidity level, we start PM* measurement. Raw data is then sent to backend using simple HTTP GET call. For every monitoring station we can apply individual calibration functions before data is presented.

## Software updates
Every sensor can be updated over-the-air. Once a day sensor sends special request to backend asking for new compiled software image. Using combination of hardware versions and software version for given harware backend decides to send new image to sensor over HTTP. 
Before applying new image we launch pre-update funcktions ensuring that even if update goes wrong, sensor is safe, e.g. we switch off the heater. Once the update is succesfull, we use post-update function to get back all sensor features.
