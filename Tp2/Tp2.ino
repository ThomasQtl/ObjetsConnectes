#include "Wire.h"
#include "SHT31.h"
#include <Arduino.h>
#include "MyModem.h"

#define SHT31_ADDRESS 0x44

#include "MKRWAN.h"

MyModem modem;
SHT31 sht;
// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
/*
uint8_t appEui[8] = {0x46, 0xFE, 0x47, 0xF0, 0x4A, 0x96, 0x26, 0xD5};
uint8_t appKey[16] = {0x73, 0x9C, 0x4, 0x77, 0xC0, 0x13, 0x36, 0x27, 0xE9, 0x83, 0xF9, 0x9D, 0x90, 0x1B, 0x9C, 0x1A};*/
String devEui;
String appEui;
String appKey;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Wire.begin();
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);
  sht.setHeatTimeout(60);

  sht.heatOn();

  devEui = "7A2732BE6BF39F17";
  appEui = "07C671C17AD715D5";
  appKey = "32A43246F0520583DABACAAAA82FAC1A";


  modem.writeAt(0, "test\r");  
  String state = modem.readAt(0);
  Serial.println(state);

/*
  char EUI[16];
  modem.deviceEUI().toCharArray(EUI, 16);

  for(int i=0; i < 8; i++)
  {
    appEui[i] = appEui[i] ^ ((uint8_t*)EUI)[i];
  }
  for(int i=0; i < 16; i++)
  {
    appKey[i] = appKey[i] ^ ((uint8_t*)EUI)[i];
  }*/

  int connected = modem.joinOTAA(appEui.c_str(), appKey.c_str(), devEui.c_str());
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

  
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  Serial.println("Connected !");
  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independent of this setting, the modem will
  // not allow sending more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.

  
}

void loop() {

  int8_t datas[10];
  for (int i=0; i<10; i++)
   {
      sht.read();
      datas[i] = (int8_t)sht.getTemperature();
      delay(3000);
   }
  int err;
  modem.beginPacket();
  modem.write((uint8_t*)datas, 10);
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
  delay(1000);
  if (!modem.available()) {
    Serial.println("No downlink message received at this time.");
    return;
  }
}