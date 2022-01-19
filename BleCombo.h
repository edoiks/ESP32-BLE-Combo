#ifndef ESP32_BLE_COMBO_H
#define ESP32_BLE_COMBO_H
#include "BleConnectionStatus.h"
#include "BLEHIDDevice.h"
#include "BLECharacteristic.h"

#define MAX_DEV_NUM 5
class BleConnectionStatus;

class BleComboInput;

class HIDInputDevice
{
public:
    virtual void createInputDev(BleComboInput*) = 0;
    virtual uint8_t getId() = 0;
    virtual void setId(uint8_t) = 0;
    virtual const uint8_t* getDescriptor() = 0;
    virtual size_t getSizeOfDescriptor() = 0;
    virtual void notify() = 0;
    virtual void disconnect() = 0;
    virtual void connect() = 0;
    bool isConnected() { return true;};
};


class BleComboInput
{
public:
  BleComboInput(std::string deviceName = "Combined BLE device", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
  void begin(void);
  void end(void);
  bool isConnected(void);
  void setBatteryLevel(uint8_t level);
  void registerDevice(HIDInputDevice* in)
  {
    if (devCount<5)
    {
        devices[devCount++] = in;
    }    
  };
  static void taskServer(void* pvParameter);

  uint8_t batteryLevel;
  BLEHIDDevice* hid;
  unsigned devCount;
  std::string deviceManufacturer;
  std::string deviceName;
  BleConnectionStatus* connectionStatus;
  HIDInputDevice** devices;
};

#endif
