#include <Arduino.h>
#include "AiEsp32RotaryEncoder.h"

// https://github.com/igorantolic/ai-esp32-rotary-encoder
// A good example...
// https://github.com/igorantolic/ai-esp32-rotary-encoder/blob/master/README_old.md


#define ENCODER_CLK D2
#define ENCODER_DT D1
//#define ENCODER_SW D0
#define ENCODER_SW D4
#define ENCODER_VCC -1
#define ENCODER_STEPS 4

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW, ENCODER_VCC, ENCODER_STEPS);

//volatile unsigned long encoderLastTurn = 0;
volatile unsigned long encoderLastPush = 0; // A variable to save the last timestamp when the button was pushed

void encoderOnButtonClick() {

  //We need to ignore any button pushes that are super quick (500 milliseconds on less)
  if (millis() - encoderLastPush < 500) {
    return;
  }

  // Get the current timestamp, save it as a variable and print it out to console
  // This is just an example of what you could do here.
  encoderLastPush = millis();
  Serial.print("button pressed ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
}

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

void setup() {

  // Set the encoder pins to inputs with an internal pullup resistor. The rotary encoder simply "shorts" the 
  // CLK, DT, and SW pins to GND whenever applicable, so we need to provide the "pull to logic HIGH" function. 
  // If we don't the pins could be "floating" somewhere between logic HIGH (3.3V) and logic LOW (0V/GND).
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, OUTPUT);

  Serial.println("Hello from line 49");

  // Initialize the rotary encoder library
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);

  // Set boundaries and if values should cycle or not
  // in this example we will set possible values between 0 and 1000;
  rotaryEncoder.setBoundaries(0, 1000, false);

  // Set a default "acceleration" behavior. Read the library to find out more about what this does.
  //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
  rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
}

void loop() {

  // Service the rotary encoder - read it's current value and print out to console
  if (rotaryEncoder.encoderChanged()) {
    Serial.print("Value: ");
    Serial.println(rotaryEncoder.readEncoder());
  }

  // Check to see if the button was clicked.  If yes, then call our function encoderOnButtonClick() and do something
  if (rotaryEncoder.isEncoderButtonClicked()) {
    encoderOnButtonClick();
  }

}