/**
 * This example turns the ESP32 into a Bluetooth LE keyboard and mouse that writes 
 * some words, presses Enter, presses a media key and then Ctrl+Alt+Delete,
 * then moves and the scrolls the mouse and clicks it.
*/
 
#include <BleCombo.h>
#include <BleComboKeyboard.h>
#include <BleComboGamepad.h>
#include <BleComboMouse.h>

#define BUTTONPIN1  21    // Pin button is attached to

int previousButton1State = HIGH;

BleComboInput comboInput;
BleComboKeyboard keyboard;
BleComboMouse mouse;
BleComboGamepad gamepad;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting work!");
  pinMode(BUTTONPIN1, INPUT_PULLUP);
  comboInput.registerDevice(&keyboard);
  comboInput.registerDevice(&mouse);
  comboInput.registerDevice(&gamepad);

  comboInput.begin();
}

void loop() {
  if(comboInput.isConnected()) {
    unsigned long startTime=0;
  
    int currentButton1State = digitalRead(BUTTONPIN1);

    if (currentButton1State != previousButton1State){
      if(currentButton1State == LOW){
        startTime = millis();
        
        Serial.println("Button Pressed sending keyboard");
        while(millis()<startTime+2000) {
          keyboard.write('A');
          delay(20);
        }
      }
    }
  }
  Serial.println("Waiting 2 seconds...");
  delay(2000);
}


