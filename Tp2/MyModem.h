#include "MKRWAN.h"


class MyModem : public LoRaModem
{
public:
  String readAt(int addr)
  {
    String value = "y a rien";
    sendAT("$NVM",addr);
    Serial.println(waitResponse());
    if (waitResponse("+OK=") == 1) {
        value = stream.readStringUntil('\r');
    }
    return value;
  }

  void writeAt(int addr, String value)
  {
    sendAT("$NVM",addr, value);
  }
};