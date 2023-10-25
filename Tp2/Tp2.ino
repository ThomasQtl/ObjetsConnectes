#include "Wire.h"
#include "SHT31.h"
#include <Arduino.h>
#include "MyModem.h"

#define SHT31_ADDRESS 0x44

MyModem modem;
SHT31 sht;
// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);
uint8_t MAGIC = 0x92;
uint8_t START_KEY_ADDR = 10;

void setup()
{
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

  
  uint8_t devEui[8];
  uint8_t appEui[8];
  uint8_t appKey[16];

  bool verif = ReadMemory(devEui,  appEui, appKey);
  if(!verif)
  {
    AdkKeys(devEui,  appEui, appKey);
  }
  Serial.println("Try connection");
  int connected = modem.joinOTAA((char*)appEui, (char*)appKey, (char*)devEui);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

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

bool ReadMemory(uint8_t* devEuiT,  uint8_t* appEuiT,  uint8_t* appKeyT)
{
  uint16_t somme = MAGIC;
  uint8_t conv;
  uint8_t addr = START_KEY_ADDR;

  uint8_t state = modem.readAt(0);

  //DEV EUI
  for(int i =0; i < 8; i+=2)
  {
      conv = modem.readAt(addr);
      somme += conv;
      devEuiT[i] = conv;
      addr++;
  }
    
  //APP EUI
  for(int i =0; i < 8; i+=2)
  {
      conv = modem.readAt(addr);
      somme += conv;
      appEuiT[i] = conv;
      addr++;
  }
  
  //APP KEY
  for(int i =0; i < 16; i+=2)
  {
      conv = modem.readAt(addr);
      somme += conv;
      appKeyT[i] = conv;
      addr++;
  }

  uint8_t checksommeH = modem.readAt(1);
  uint8_t checksommeL = modem.readAt(2);

  uint8_t xlow = somme & 0xff;
  uint8_t xhigh = (somme >> 8);

  if(state != MAGIC || checksommeH != xhigh || checksommeL != xlow)
  {
    return false;
  }
  return true;

}

void AdkKeys(uint8_t* devEuiT,  uint8_t* appEuiT,  uint8_t* appKeyT)
{
    uint8_t addr = START_KEY_ADDR;
    uint8_t conv;
    uint16_t somme = MAGIC;

    String devEui;
    String appEui;
    String appKey;

    //DEV EUI
    Serial.println("devEui in hex");
    while(!Serial.available()) {}
    devEui = Serial.readString();
    for(int i =0; i < 8; i+=2)
    {
        conv = StrToHex(devEui.substring(i,i+2).c_str()); 
        somme += conv;
        devEuiT[i] = conv;
        modem.writeAt(addr, conv);
        addr++;
    }
    
    //APP EUI
    Serial.println("appEui in hex");
    while(!Serial.available()) {}
    appEui = Serial.readString();
    for(int i =0; i < 8; i+=2)
    {
        conv = StrToHex(appEui.substring(i,i+2).c_str());  
        somme += conv;
        appEuiT[i] = conv;
        modem.writeAt(addr, conv);
        addr++;
    }
    
    //APP KEY
    Serial.println("appEui key in hex");
    while(!Serial.available()) {}
    appKey = Serial.readString();
    for(int i =0; i < 16; i+=2)
    {
        conv = StrToHex(appKey.substring(i,i+2).c_str());  
        somme += conv;
        appKeyT[i] = conv;
        modem.writeAt(addr, conv);
        addr++;
    }

    uint8_t xlow = somme & 0xff;
    uint8_t xhigh = (somme >> 8);

    modem.writeAt(0,MAGIC);
    modem.writeAt(1,xhigh);
    modem.writeAt(2,xlow);

  modem.lockKey();
}

uint8_t StrToHex(const char* str)
{
  return (uint8_t) strtoull(str, 0, 16);
}