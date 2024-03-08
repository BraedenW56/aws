#include <Arduino.h>
#include <U8g2lib.h>
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

// Specify which pins are used on the microcontroller to communicate with the OLED display
#define OLED_CS D8 // clock
#define OLED_RES D3 // reset
//#define OLED_DC D4 // data
#define OLED_DC D0 // data

// Constructor - This initializes "oled" so now you can use it to call functions (e.g., oled.drawStr)
// See this link for a list of all the functions that can be used
// https://github.com/olikraus/u8g2/wiki/u8g2reference
U8G2_SSD1309_128X64_NONAME2_1_4W_HW_SPI oled(U8G2_R0, OLED_CS, OLED_DC, OLED_RES);

//Encoder constructor
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW, ENCODER_VCC, ENCODER_STEPS);

// Variables for drawing the wire at the top of the OLED screen.
u8g2_uint_t SCREEN_WIDTH = 0;
u8g2_uint_t SCREEN_HEIGHT = 0;
const int WIRE_STRAND_LENGTH = 30;
const int WIRE_STRAND_Y_POS = 7;
const int WIRE_INSULATION_WIDTH = SCREEN_WIDTH - (WIRE_STRAND_LENGTH * 2);
const int WIRE_INSULATION_HEIGHT = 14;

//volatile unsigned long encoderLastTurn = 0;
volatile unsigned long encoderLastPush = 0; // A variable to save the last timestamp when the button was pushed
volatile unsigned long strip_length_left = 0; 

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

void encoder_setup() {

  // Set the encoder pins to inputs with an internal pullup resistor. The rotary encoder simply "shorts" the 
  // CLK, DT, and SW pins to GND whenever applicable, so we need to provide the "pull to logic HIGH" function. 
  // If we don't the pins could be "floating" somewhere between logic HIGH (3.3V) and logic LOW (0V/GND).
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, OUTPUT);

  Serial.begin(9600);
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

// The setup function will execute once on the microcontroller right after boot-up or on reset
void oled_setup() {

  // Initialize the OLED display using the oled.begin() function - this is necessary.
  oled.begin();

  // Set the font and display rotation
  oled.setFont(u8g2_font_logisoso16_tf);
  oled.setDisplayRotation(U8G2_R0); // U8G2_R0-no rotation

  // Precompute some variables - No point in doing these things in the "loop" section 
  // since the results would never change
  // Get the actual display width and height so we can compute pixel locations later
  SCREEN_HEIGHT = oled.getDisplayHeight();
  SCREEN_WIDTH = oled.getDisplayWidth();
}

// Microcontroller will loop indefinitely
void oled_update() {

  // Clear the display
  oled.clearDisplay();
    
  // firstPage() must always be used in a do-while loop with the nextPage() command - this renders the content of the display
  oled.firstPage();
  do {
    // All graphics commands have to appear within the do-while loop body

    // An example, just display "HELLO WORLD" using the drawStr() function, once you feel comfortable delete this line and work on drawWire() function
    //oled.drawStr(0, 20, "HELLO WORLD");
    oled.drawLine(0, 7, 128, 7);
    oled.drawBox(34,5,58,5);
    oled.drawFrame(0,16,41,19);
    oled.drawFrame(42,16,42,19);
    oled.drawFrame(85,16,41,19);
    oled.drawFrame(0,38,41,19);
    oled.drawFrame(42,38,42,19);
    oled.drawFrame(85,38,41,19);
    oled.setCursor(2, 33);
    oled.print(strip_length_left);
    // This will be part of your first task...
    //oled.drawWire();

  } while (oled.nextPage());

}

bool read_encoder() {

  bool haschanged = false;

  // Service the rotary encoder - read it's current value and print out to console
  if (rotaryEncoder.encoderChanged()) {
    Serial.print("Value: ");
    Serial.println(rotaryEncoder.readEncoder());
    strip_length_left = rotaryEncoder.readEncoder();
    haschanged = true;
  }

  // Check to see if the button was clicked.  If yes, then call our function encoderOnButtonClick() and do something
  if (rotaryEncoder.isEncoderButtonClicked()) {
    encoderOnButtonClick();
  }
return haschanged;
}

void setup() {
  oled_setup();
  encoder_setup();
  oled_update();
}

void loop() {
  
	//optionally we can ignore whenever there is no change
	if (read_encoder()) {
    oled_update();
  }
}