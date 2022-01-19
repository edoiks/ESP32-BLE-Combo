#include "BleConnectionStatus.h"

BleConnectionStatus::BleConnectionStatus(HIDInputDevice** in, unsigned* count)
{
    devices = in;    
    devCount = count;
}

void BleConnectionStatus::onConnect(BLEServer* pServer)
{
  this->connected = true;
  for (unsigned i=0; i<*devCount; i++)
  {
    if (devices[i] != nullptr)
    {
        devices[i]->connect();
    }
  }
}

void BleConnectionStatus::onDisconnect(BLEServer* pServer)
{
  this->connected = false;
  for (unsigned i=0; i<*devCount; i++)
  {
    if (devices[i] != nullptr)
    {
        devices[i]->disconnect();
    }
  }
}
