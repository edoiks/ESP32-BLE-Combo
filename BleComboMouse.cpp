#include "BleComboMouse.h"

#define MOUSE_ID 0x03

static const uint8_t mouseHidReportDescriptor[] = {
  // ------------------------------------------------- Mouse
  USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x02, // USAGE (Mouse)
  COLLECTION(1),       0x01, // COLLECTION (Application)
  REPORT_ID(1),        MOUSE_ID, //     REPORT_ID (1)
  USAGE(1),            0x01, //   USAGE (Pointer)
  COLLECTION(1),       0x00, //   COLLECTION (Physical)
  // ------------------------------------------------- Buttons (Left, Right, Middle, Back, Forward)
  USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
  USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
  USAGE_MAXIMUM(1),    0x05, //     USAGE_MAXIMUM (Button 5)
  LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x05, //     REPORT_COUNT (5)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;5 button bits
  // ------------------------------------------------- Padding
  REPORT_SIZE(1),      0x03, //     REPORT_SIZE (3)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;3 bit padding
  // ------------------------------------------------- X/Y position, Wheel
  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x30, //     USAGE (X)
  USAGE(1),            0x31, //     USAGE (Y)
  USAGE(1),            0x38, //     USAGE (Wheel)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x03, //     REPORT_COUNT (3)
  HIDINPUT(1),         0x06, //     INPUT (Data, Variable, Relative) ;3 bytes (X,Y,Wheel)
  // ------------------------------------------------- Horizontal wheel
  USAGE_PAGE(1),       0x0c, //     USAGE PAGE (Consumer Devices)
  USAGE(2),      0x38, 0x02, //     USAGE (AC Pan)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  HIDINPUT(1),         0x06, //     INPUT (Data, Var, Rel)
  END_COLLECTION(0),
  END_COLLECTION(0)         //   END_COLLECTION
};

const uint8_t* BleComboMouse::getDescriptor()
{
    return mouseHidReportDescriptor;
}

size_t BleComboMouse::getSizeOfDescriptor()
{
  return sizeof(mouseHidReportDescriptor);
};

void BleComboMouse::click(uint8_t b)
{
  buttons = b;
  move(0,0,0,0);
  buttons = 0;
  move(0,0,0,0);
}

void BleComboMouse::move(signed char x, signed char y, signed char wheel, signed char hWheel)
{
  if (isConnected())
  {
    uint8_t m[5];
    m[0] = buttons;
    m[1] = x;
    m[2] = y;
    m[3] = wheel;
    m[4] = hWheel;
    devHook->setValue(m, 5);
    notify();
  }
}

void BleComboMouse::checkButtons(uint8_t b)
{
  if (b != buttons)
  {
    buttons = b;
    move(0,0,0,0);
  }
}

void BleComboMouse::press(uint8_t b)
{
  checkButtons(buttons | b);
}

void BleComboMouse::release(uint8_t b)
{
  checkButtons(buttons & ~b);
}

bool BleComboMouse::isPressed(uint8_t b)
{
  return ((b & buttons) > 0);
}

void BleComboMouse::createInputDev(BleComboInput* handler)
{
  devHook = handler->hid->inputReport(MOUSE_ID); // <-- input REPORTID from report map
} 

