#include "SmoglyDHT.h"
#include <DHT.h>
#define DHTPIN D3     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT local_dht(DHTPIN, DHTTYPE);

void SmoglyDHT::setup() {
  local_dht.begin();
}

float SmoglyDHT::readHumidity() {
  return local_dht.readHumidity();
}

float SmoglyDHT::readTemperature() {
  return local_dht.readTemperature();
}
