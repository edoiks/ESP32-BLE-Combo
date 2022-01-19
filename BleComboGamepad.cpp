#include "BleComboGamepad.h"
#include "HidHelper.h"

#define GAMEPAD_ID 0x04
uint8_t gampadHidReportDescriptor[150];

void BleComboGamepad::build()
{
  
  uint8_t buttonPaddingBits = 8 - (numberOfButtons % 8);
  if(buttonPaddingBits == 8)
  {
    buttonPaddingBits = 0;
  }
  numberOfButtonsBytes = numberOfButtons / 8;
  if( buttonPaddingBits > 0)
  {
    numberOfButtonsBytes++;
  }

  reportSize = numberOfButtonsBytes+numberOfHatButtons;

  setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, USAGE_PAGE(1), 0x01);
  setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, USAGE(1), 0x05); // (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
  setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, COLLECTION(1), 0x01);
  setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, REPORT_ID(1), GAMEPAD_ID);
  setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, COLLECTION(1), 0x00);

  if (numberOfButtons>0)
  {
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, USAGE_PAGE(1), 0x09); // Button
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, USAGE_MINIMUM(1), 0x01); // From Button 1
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, USAGE_MAXIMUM(1), numberOfButtons); // To Botton 10
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, LOGICAL_MINIMUM(1), 0x00);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, LOGICAL_MAXIMUM(1), 0x01);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, REPORT_COUNT(1), numberOfButtons);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, REPORT_SIZE(1), 0x01);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, UNIT_EXPONENT(1), 0x00);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, UNIT(1), 0x00);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, HIDINPUT(1), 0x02);

    if (buttonPaddingBits > 0)
    {
     setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, REPORT_COUNT(1), 0x01);
     setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, REPORT_SIZE(1), 0x06);  // padding bits 10 buttons
     setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, HIDINPUT(1), 0x03);
    }
  }

  if (numberOfHatButtons>0)
  {
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, COLLECTION(1), 0x00);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, USAGE_PAGE(1), 0x01); // Button
    for(int currentHatIndex = 0 ; currentHatIndex < numberOfHatButtons ; currentHatIndex++)
    {
      setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, USAGE(1), 0x39);
    }
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, LOGICAL_MINIMUM(1), 0x00);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, LOGICAL_MAXIMUM(1), 0x01);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, PHYSICAL_MINIMUM(1), 0x00);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, PHYSICAL_MAXIMUM(2), 0x3B);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, 0x01);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, UNIT(1), 0x12);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, REPORT_SIZE(1), 0x08);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, REPORT_COUNT(1), numberOfHatButtons);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, HIDINPUT(1), 0x42);
    setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, END_COLLECTION(0));
  }
  
  setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, END_COLLECTION(0));
  setValueInDescriptor(gampadHidReportDescriptor, hidDescriptorSize, END_COLLECTION(0));
 
}

size_t BleComboGamepad::getSizeOfDescriptor() 
{ 
  return hidDescriptorSize;
}; 

const uint8_t* BleComboGamepad::getDescriptor()
{
    return gampadHidReportDescriptor;
}

void BleComboGamepad::sendReport()
{
  if (isConnected())
  {
    uint8_t currentReportIndex = 0;
    uint8_t m[reportSize];
    memset(&m, 0, sizeof(m));
    memcpy(&m, &buttons, sizeof(numberOfButtonsBytes));
    currentReportIndex+=numberOfButtonsBytes;
    if (numberOfHatButtons>0)
    {
      m[currentReportIndex++] = hat1;
    }
    devHook->setValue(m, sizeof(m));
    notify();
  }
}

void BleComboGamepad::press(uint8_t b)
{
  uint8_t index = (b-1) / 8;
  uint8_t bit = (b-1) % 8;
  uint8_t bitmask = (1 << bit);

  uint8_t result = buttons[index] | bitmask;
  
  if (result != buttons[index]) 
  {
    buttons[index] = result;
  }
 
  sendReport();
}

void BleComboGamepad::release(uint8_t b)
{
  uint8_t index = (b-1) / 8;
  uint8_t bit = (b-1) % 8;
  uint8_t bitmask = (1 << bit);

  uint64_t result = buttons[index] & ~bitmask;
  
  if (result != buttons[index]) 
  {
    buttons[index] = result;
  }
  
  sendReport();
}

bool BleComboGamepad::isPressed(uint8_t b)
{
  uint8_t index = (b-1) / 8;
  uint8_t bit = (b-1) % 8;
  uint8_t bitmask = (1 << bit);

  if ((bitmask & buttons[index]) > 0)
    return true;
  return false;
}

void BleComboGamepad::createInputDev(BleComboInput* handler)
{
  devHook = handler->hid->inputReport(GAMEPAD_ID);
}

