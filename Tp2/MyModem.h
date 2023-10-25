#include "MKRWAN.h"


class MyModem : public LoRaModem
{
public:
  uint8_t readAt(int addr)
  {
    String value = "";
    sendAT("$NVM "+ String(addr));
    if (waitResponse() == 1) {
      value = stream.readStringUntil('\n');
    }
    return value.substring(1).toInt();
  }

  void writeAt(int addr, int value)
  {
    String mess = String(addr) + "," + String(value);
    sendAT("$NVM "+mess);
    waitResponse();
  }

  void writeAt(String mess)
  {
    sendAT("$NVM "+mess);
    waitResponse();
  }

  void lockKey()
  {
    sendAT("$APKACCESS");
    waitResponse();
  }
};