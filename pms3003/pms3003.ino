/*
 This example demonstrate how to read pm2.5 value on PMS 3003 air condition sensor

 PMS 3003 pin map is as follow:
    PIN1  :VCC, connect to 5V
    PIN2  :GND
    PIN3  :SET, 0:Standby mode, 1:operating mode
    PIN4  :RXD :Serial RX
    PIN5  :TXD :Serial TX
    PIN6  :RESET
    PIN7  :NC
    PIN8  :NC

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

#define PMS_COMMAND1 0
#define PMS_COMMAND2 1
#define PMS_PM1C_HIGH 2
#define PMS_PM1C_LOW 3
#define PMS_PM25C_HIGH 4
#define PMS_PM25C_LOW 5
#define PMS_PM10C_HIGH 6
#define PMS_PM10C_LOW 7
#define PMS_PM1_HIGH 8
#define PMS_PM1_LOW 9
#define PMS_PM25_HIGH 10
#define PMS_PM25_LOW 11
#define PMS_PM10_HIGH 12
#define PMS_PM10_LOW 13
#define PMS_RES1_HIGH 14
#define PMS_RES1_LOW 15
#define PMS_RES2_HIGH 16
#define PMS_RES2_LOW 17
#define PMS_RES3_HIGH 18
#define PMS_RES3_LOW 19
#define PMS_CHECKSUM_HIGH 20
#define PMS_CHECKSUM_LOW 21

SoftwareSerial pms(PMS_PIN_RX, PMS_PIN_TX); // Initialize serial communiation with PMS 3003

#define pmsDataLen 22
uint8_t buf[pmsDataLen];
int idx = 0;
int pm10 = 0;
int pm25 = 0;

void setup() {
  Serial.begin(115200);
  pms.begin(PMS_BAUDRATE); // PMS 3003 UART has baud rate 9600
}

void loop() { // run over and over
  uint8_t c = 0;
  idx = 0;
  memset(buf, 0, pmsDataLen);

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
  long pms_calc_checksum = 0x42 + 0x4d + buf[PMS_COMMAND2] + buf[PMS_COMMAND1] + buf[PMS_PM1C_LOW] + buf[PMS_PM25C_LOW] + buf[PMS_PM10C_LOW] + buf[PMS_PM1_LOW] + buf[PMS_PM25_LOW] + buf[PMS_PM10_LOW] + buf[PMS_RES1_LOW] + buf[PMS_RES2_LOW] + buf[PMS_RES3_LOW] +
                                   buf[PMS_PM1C_HIGH] + buf[PMS_PM25C_HIGH] + buf[PMS_PM10C_HIGH] + buf[PMS_PM1_HIGH] + buf[PMS_PM25_HIGH] + buf[PMS_PM10_HIGH] + buf[PMS_RES1_HIGH] + buf[PMS_RES2_HIGH] + buf[PMS_RES3_HIGH];

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
    for (int i=0; i< pmsDataLen;i++) {
      Serial.print(buf[i]);
      Serial.print("..");
    }
    Serial.println();
    }

  pm10 = ( buf[10] << 8 ) | buf[11];
  pm25 = ( buf[12] << 8 ) | buf[13];

  Serial.print("pm2.5: ");
  Serial.print(pm25);
  Serial.print(" ug/m3   ");
  Serial.print("pm10: ");
  Serial.print(pm10);
  Serial.println(" ug/m3   ");
  delay(5000);
}
