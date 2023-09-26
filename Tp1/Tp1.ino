#include "Wire.h"
#include "SHT31.h"
#include <Arduino.h>
#include "disk91_LoRaE5.h"

Disk91_LoRaE5 lorae5(false);

#define SHT31_ADDRESS   0x44

SHT31 sht;
uint16_t status;

uint8_t deveui[] = {0x4A, 0x23, 0x26, 0x94, 0x13, 0x65, 0x82, 0x6C};
uint8_t appeui[] = {0x07, 0xC6, 0x71, 0xC1, 0x7A, 0xD7, 0x15, 0xD5};
uint8_t appkey[] = {0x32, 0xA4, 0x32, 0x46, 0xF0, 0x52, 0x05, 0x83, 0xDA, 0xBA, 0xCA, 0xAA, 0xA8, 0x2F, 0xAC, 0x1A};

void setup()
{
  Serial.begin(9600);
  uint32_t start = millis();
  while ( !Serial && (millis() - start) < 1500 );  // Open the Serial Monitor to get started or wait for 1.5"

  // init the library, search the LORAE5 over the different WIO port available
  if ( ! lorae5.begin(DSKLORAE5_SEARCH_WIO) ) {
    Serial.println("LoRa E5 Init Failed");
    while(1); 
  }

  // Setup the LoRaWan Credentials
  if ( ! lorae5.setup(
          DSKLORAE5_ZONE_EU868,     // LoRaWan Radio Zone EU868 here
          deveui,
          appeui,
          appkey
       ) ){
    Serial.println("LoRa E5 Setup Failed");
    while(1);         
  }
}

void loop() {
  static uint8_t data[] = { 0x01, 0x02, 0x03, 0x04 }; 

  // Send an uplink message. The Join is automatically performed
  if ( lorae5.send_sync(
        1,              // LoRaWan Port
        data,           // data array
        sizeof(data),   // size of the data
        false,          // we are not expecting a ack
        7,              // Spread Factor
        14              // Tx Power in dBm
       ) 
  ) {
      Serial.println("Uplink done");
      if ( lorae5.isDownlinkReceived() ) {
        Serial.println("A downlink has been received");
        if ( lorae5.isDownlinkPending() ) {
          Serial.println("More downlink are pending");
        }
      }
  }
  delay(30000);
    
}

/*
void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("SHT31_LIB_VERSION: \t");
  Serial.println(SHT31_LIB_VERSION);

  Wire.begin();
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);

  sht.setHeatTimeout(60);  // heater timeout 30 seconds, just for demo.

  status = sht.readStatus();
  printHeaterStatus(status);

  sht.heatOn();

  while (sht.isHeaterOn())
  {
    status = sht.readStatus();
    printHeaterStatus(status);
    sht.read();
    Serial.print("Temperature :");
    Serial.println(sht.getTemperature());
    delay(1000);
  }

  Serial.println("switched off");
}


void loop()
{
  // forced switch off
  if (status & SHT31_STATUS_HEATER_ON) sht.heatOff();
}


void printHeaterStatus(uint16_t status)
{
  Serial.print(millis());
  Serial.print("\tHEATER: ");
  if (status & SHT31_STATUS_HEATER_ON)
  {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }
}
*/
