#ifndef ESP32_BLE_CONNECTION_STATUS_H
#define ESP32_BLE_CONNECTION_STATUS_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include <BLEServer.h>
#include "BleCombo.h"
class HIDInputDevice;

class BleConnectionStatus : public BLEServerCallbacks
{
public:
  BleConnectionStatus(HIDInputDevice** in, unsigned* count);
  bool connected = false;
  void onConnect(BLEServer* pServer);
  void onDisconnect(BLEServer* pServer);
  unsigned* devCount;
  HIDInputDevice** devices;
};

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_CONNECTION_STATUS_H
