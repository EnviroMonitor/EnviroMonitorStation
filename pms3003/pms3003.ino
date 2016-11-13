/*
This example demonstrate how to read pm1, pm2.5 and pm10 values from PMS 3003 air condition sensor.

Inspired by:
 https://github.com/brucetsao/eParticle/blob/master/PMS3003AirQualityforLinkIt/PMS3003AirQualityforLinkIt.ino and Dariusz Borowski code https://bitbucket.org/dariusz-borowski/obywatelski-monitoring-pylu

 PMS 3003 pin map is as follow:
    PIN1  :VCC, connect to 5V
    PIN2  :GND, connect to GND from power source
    PIN3  :SET, 0:Standby mode, 1:operating mode
    PIN4  :RXD :Serial RX
    PIN5  :TXD :Serial TX
    PIN6  :RESET
    PIN7  :NC
    PIN8  :NC

 Data format of PMS3003 is 24 one byte messages:
 Header 1
 Header 2
 Control bit high = 2 x 9 + 2
 Control bit low
 PM1.0 high standard particle
 PM1.0 low standard particle
 PM2.5 high standard particle
 PM2.5 low standard particle
 PM10 high standard particle
 PM10 low standard particle
 PM1.0 high
 PM1.0 low
 PM2.5 high
 PM2.5 low
 PM10 high
 PM10 low
 Reserved 1 high
 Reserved 1 low
 Reserved 2 high
 Reserved 2 low
 Reserved 3 high
 Reserved 4 low
 Checksum high - sum of 9 previous readings
 Checksum low
 

 In this example, we only use Serial to get PM 2.5 value.

 The circuit:
 * RX is digital pin 6 (connect to TX of PMS 3003)
 * TX is digital pin 7 (connect to RX of PMS 3003)
 * SET is digital pin 5 (connect to SET of PMS 3003)

 */

#include <SoftwareSerial.h>

#define PMS_PIN_SET D5
#define PMS_PIN_RX D7
#define PMS_PIN_TX D6
#define PMS_BAUDRATE 9600

// Data format
#define PMS_HEADER1 0 //0x42
#define PMS_HEADER2 1 // 0x4d
#define PMS_COMMAND1 2 // frame lenght high
#define PMS_COMMAND2 3 // frame lenght low
#define PMS_PM1C_HIGH 4
#define PMS_PM1C_LOW 5
#define PMS_PM25C_HIGH 6
#define PMS_PM25C_LOW 7
#define PMS_PM10C_HIGH 8
#define PMS_PM10C_LOW 9
#define PMS_PM1_HIGH 10
#define PMS_PM1_LOW 11
#define PMS_PM25_HIGH 12
#define PMS_PM25_LOW 13
#define PMS_PM10_HIGH 14
#define PMS_PM10_LOW 15
#define PMS_RES1_HIGH 16
#define PMS_RES1_LOW 17
#define PMS_RES2_HIGH 18
#define PMS_RES2_LOW 19
#define PMS_RES3_HIGH 20
#define PMS_RES3_LOW 21
#define PMS_CHECKSUM_HIGH 22
#define PMS_CHECKSUM_LOW 23

SoftwareSerial pms(PMS_PIN_RX, PMS_PIN_TX); // Initialize serial communiation with PMS 3003

#define pmsDataLen 24 // according to spec PMS3003 has 24 bytes long message
uint8_t buf[pmsDataLen];
int idx = 0;
long pm10 = 0;
long pm25 = 0;

void setup() {
  Serial.begin(115200);
  pms.begin(PMS_BAUDRATE); // PMS 3003 UART has baud rate 9600
  pinMode(PMS_PIN_SET, OUTPUT);
  digitalWrite(PMS_PIN_SET, HIGH);
}

void loop() { // run over and over
  uint8_t c = 0;
  idx = 0;
  memset(buf, 0, pmsDataLen);

  //digitalWrite(PMS_PIN_SET, HIGH);
  delay(1000);

  while (true) {
    while (c != 0x42) {
      while (!pms.available());
      c = pms.read();
    }
    while (!pms.available());
    c = pms.read();
    if (c == 0x4d) {
      // now we got a correct header)
      buf[idx++] = 0x42;
      buf[idx++] = 0x4d;
      break;
    }
  }

  while (idx != pmsDataLen) {
    while(!pms.available());
    buf[idx++] = pms.read();
  }
  long pms_checksum = word(buf[PMS_CHECKSUM_HIGH], buf[PMS_CHECKSUM_LOW]);
  long pms_calc_checksum = buf[PMS_HEADER1] + buf[PMS_HEADER2] + 
                            buf[PMS_COMMAND1] + buf[PMS_COMMAND2] + 
                            buf[PMS_PM1C_HIGH] + buf[PMS_PM1C_LOW] + 
                            buf[PMS_PM25C_HIGH] + buf[PMS_PM25C_LOW] + 
                            buf[PMS_PM10C_HIGH] + buf[PMS_PM10C_LOW] + 
                            buf[PMS_PM1_HIGH] + buf[PMS_PM1_LOW] + 
                            buf[PMS_PM25_HIGH] + buf[PMS_PM25_LOW] + 
                            buf[PMS_PM10_HIGH] + buf[PMS_PM10_LOW] + 
                            buf[PMS_RES1_HIGH] + buf[PMS_RES1_LOW] + 
                            buf[PMS_RES2_HIGH] + buf[PMS_RES2_LOW] +
                            buf[PMS_RES3_HIGH] + buf[PMS_RES3_LOW];


  if (pms_calc_checksum == pms_checksum) {
    Serial.print("Checksum correct: ");
    Serial.print(pms_checksum);
    Serial.print(" / ");
    Serial.println(pms_calc_checksum);
  }
  else {
    Serial.print("Checksum incorrect: ");
    Serial.print(pms_checksum);
    Serial.print(" / ");
    Serial.println(pms_calc_checksum);
    Serial.println("Raw data dump:");
    for (int i=0; i< pmsDataLen;i++) {
      Serial.print(buf[i]);
      Serial.print(" ");
    }
    Serial.println();
    }

  pm25 = word(buf[12], buf[13]);
  pm10 = word(buf[14], buf[15]);
 
  Serial.print("pm2.5: ");
  Serial.print(pm25);
  Serial.print(" pm10: ");
  Serial.println(pm10);
  delay(4000);
}
