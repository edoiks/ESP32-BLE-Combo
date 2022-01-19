#ifndef ESP32_BLE_COMBO_MOUSE_H
#define ESP32_BLE_COMBO_MOUSE_H
#include "BleCombo.h"
#include "BLE2902.h"
#include <BLEServer.h>
#include "BLECharacteristic.h"

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_BACK 8
#define MOUSE_FORWARD 16
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE) # For compatibility with the Mouse library


class BleComboMouse : public HIDInputDevice
{
private:
  BLECharacteristic* devHook;
  uint8_t id;
  uint8_t buttons;
  void checkButtons(uint8_t b);
public:
  BleComboMouse() {};

  void begin(void) {};
  void end(void) {};
  virtual void notify()
  {
    devHook->notify();
  };
  virtual void createInputDev(BleComboInput*);
  
  virtual void setId(uint8_t value)
  {
    id = value;
  };
  virtual uint8_t getId()
  {
    return id;
  };
  virtual const uint8_t* getDescriptor();
  virtual size_t getSizeOfDescriptor();
  void click(uint8_t b = MOUSE_LEFT);
  void move(signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0);
  void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
  void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
  bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
  virtual void disconnect()
  {
    BLE2902* desc = (BLE2902*)devHook->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(true);
  }
  virtual void connect()
  {
    BLE2902* desc = (BLE2902*)devHook->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(false);
  }
};

#endif // ESP32_BLE_COMBO_MOUSE_H
