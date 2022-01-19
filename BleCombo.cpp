#include "BleCombo.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#include "BleConnectionStatus.h"
#include "KeyboardOutputCallbacks.h"
#include "BleComboKeyboard.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "BLEDevice";
#endif

BleComboInput::BleComboInput(std::string deviceNameToSet, std::string deviceManufacturerToSet, uint8_t initialBatteryLevel)
    : hid(0), devCount(0)
{
    deviceName = deviceNameToSet;
    deviceManufacturer = deviceManufacturerToSet;
    batteryLevel = initialBatteryLevel;
    devices = new HIDInputDevice*[MAX_DEV_NUM];
    connectionStatus = new BleConnectionStatus(devices, &devCount);
}

void BleComboInput::begin(void)
{
  xTaskCreate(taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleComboInput::end(void)
{
}

bool BleComboInput::isConnected(void) {
  return this->connectionStatus->connected;
}

void BleComboInput::setBatteryLevel(uint8_t level) {
  batteryLevel = level;
  if (hid != 0)
  {
    hid->setBatteryLevel(batteryLevel);
  }
}

void BleComboInput::taskServer(void* pvParameter)
{
  BleComboInput* inputDevice = static_cast<BleComboInput*>(pvParameter);
  BLEDevice::init(inputDevice->deviceName);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(inputDevice->connectionStatus);

  inputDevice->hid = new BLEHIDDevice(pServer);

  int descriptorsSizes[inputDevice->devCount];
  int sizeOfCombinedDescriptor=0;
  const uint8_t* descriptors[inputDevice->devCount];
  for (unsigned i=0; i<inputDevice->devCount; i++)
  {
    inputDevice->devices[i]->createInputDev(inputDevice);
    descriptorsSizes[i] = inputDevice->devices[i]->getSizeOfDescriptor();
    descriptors[i] = inputDevice->devices[i]->getDescriptor();
    sizeOfCombinedDescriptor += descriptorsSizes[i];
  };

  int front = 0; 
  uint8_t hidReportDescriptor[sizeOfCombinedDescriptor];
  for (unsigned i=0; i<inputDevice->devCount; ++i)
  { 
     memcpy(&hidReportDescriptor[front],&descriptors[i][0],descriptorsSizes[i]);
     front += descriptorsSizes[i];
  };

  inputDevice->hid->manufacturer()->setValue(inputDevice->deviceManufacturer);
  inputDevice->hid->pnp(0x02, 0xe502, 0xaddd, 0x0210);
  inputDevice->hid->hidInfo(0x00,0x01);
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
  inputDevice->hid->reportMap((uint8_t*)hidReportDescriptor, sizeOfCombinedDescriptor);
  inputDevice->hid->startServices();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_KEYBOARD);
  pAdvertising->addServiceUUID(inputDevice->hid->hidService()->getUUID());
  pAdvertising->start();
  inputDevice->hid->setBatteryLevel(inputDevice->batteryLevel);

  ESP_LOGD(LOG_TAG, "Advertising started!");
  vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}

