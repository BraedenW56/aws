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

#define ADJUST_VALUE true
#define SELECT_BOXES false

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
volatile unsigned long wire_length = 0;
volatile unsigned long strip_length_right = 0;
volatile unsigned long quantity = 0;
volatile unsigned long strip_depth = 0;
volatile unsigned short box_num = 1;

volatile bool button_mode = SELECT_BOXES;

void oled_print_box_values() {
  oled.setFont(u8g2_font_logisoso16_tf);
  oled.setCursor(1, 33);
  oled.print(strip_length_left);
  oled.setCursor(43, 33);
  oled.print(wire_length);
  oled.setCursor(86, 33);
  oled.print(strip_length_right);
  oled.setCursor(1, 58);
  oled.print(quantity);
  oled.setCursor(43, 58);
  oled.print(strip_depth);
}

void encoderOnButtonClick() {

  //We need to ignore any button pushes that are super quick (500 milliseconds on less)
  if (millis() - encoderLastPush < 500) {
    return;
  }

  if (button_mode == SELECT_BOXES) {
    button_mode = ADJUST_VALUE;
    
    if (box_num == 1){
        rotaryEncoder.setEncoderValue(strip_length_left);
    }
    if (box_num == 2){
          rotaryEncoder.setEncoderValue(wire_length);
    }
    if (box_num == 3){
          rotaryEncoder.setEncoderValue(strip_length_right);
    }
    if (box_num == 4){
          rotaryEncoder.setEncoderValue(quantity);
    }
    if (box_num == 5){
          rotaryEncoder.setEncoderValue(strip_depth);
    }
    if (box_num == 6){
          //read encoder for click to start machine
    } 

  } else {
    button_mode = SELECT_BOXES;
    rotaryEncoder.setEncoderValue(box_num - 1);
  }
  //start here

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
  oled_drawstuff();
}

// Microcontroller will loop indefinitely
void oled_update() {

  // Clear the display
  oled.clearDisplay();
    
  // firstPage() must always be used in a do-while loop with the nextPage() command - this renders the content of the display
  oled.firstPage();
  do {
    oled_drawstuff();
  } while (oled.nextPage());

}

void oled_drawstuff() {
   // All graphics commands have to appear within the do-while loop body
    // An example, just display "HELLO WORLD" using the drawStr() function, once you feel comfortable delete this line and work on drawWire() function
    //oled.drawStr(0, 20, "HELLO WORLD");
    oled.setFontMode(0);
  oled.setDrawColor(1);
    oled.drawLine(0, 7, 128, 7);
    oled.drawBox(34,5,58,5);
    oled.drawFrame(0,15,41,20);
    oled.drawFrame(42,15,42,20);
    oled.drawFrame(85,15,41,20);
    oled.drawFrame(0,40,41,20);
    oled.drawFrame(42,40,42,20);
    oled.drawFrame(85,40,41,20);
    oled.setCursor(91,54);
    oled.setFont(u8g2_font_6x13_tf);
    oled.print("Start");
    if (box_num == 1){
      oled.setFontMode(1);
      oled.setDrawColor(2);
        oled.setFontMode(1);
        oled.drawBox(0,15,41,20);
    }
    if (box_num == 2){
      oled.setFontMode(1);
      oled.setDrawColor(2);
        oled.setFontMode(1);
        oled.drawBox(42,15,41,20);
    }
    if (box_num == 3){
      oled.setFontMode(1);
      oled.setDrawColor(2);
        oled.setFontMode(1);
        oled.drawBox(85,15,41,20);
    }
    if (box_num == 4){
      oled.setFontMode(1);
      oled.setDrawColor(2);
        oled.setFontMode(1);
        oled.drawBox(0,40,41,20);
    }
    if (box_num == 5){
      oled.setFontMode(1);
      oled.setDrawColor(2);
        oled.setFontMode(1);
        oled.drawBox(42,40,41,20);
    }  
    if (box_num == 6){
      oled.setFontMode(1);
      oled.setDrawColor(2);
        oled.setFontMode(1);
        oled.drawBox(85,40,41,20);
    }
    oled_print_box_values();
    // This will be part of your first task...
    //oled.drawWire();

}

bool read_encoder_box_mode() {
  bool haschanged = false;

  // Service the rotary encoder - read it's current value and print out to console
  if (rotaryEncoder.encoderChanged()) {
    Serial.print("Value: ");
    Serial.println(rotaryEncoder.readEncoder());

    box_num = 1 + rotaryEncoder.readEncoder() % 6;

    oled_drawstuff();

    haschanged = true;
  }

  // Check to see if the button was clicked.  If yes, then call our function encoderOnButtonClick() and do something
  if (rotaryEncoder.isEncoderButtonClicked()) {
    encoderOnButtonClick();
  }
return haschanged;
}

bool read_encoder_value_mode() {

  bool haschanged = false;

  // Service the rotary encoder - read it's current value and print out to console
  if (rotaryEncoder.encoderChanged()) {
    Serial.print("Value: ");
    Serial.println(rotaryEncoder.readEncoder());

    if (box_num > 6){
      box_num = 1;      
    } 
    /*
    u8g2.setFontMode(1);  / * activate transparent font mode * /
    u8g2.setDrawColor(1); / * color 1 for the box * /
    u8g2.drawBox(22, 2, 35, 50);
    u8g2.setFont(u8g2_font_ncenB14_tf);
    u8g2.setDrawColor(0);
    u8g2.drawStr(5, 18, "abcd");
    u8g2.setDrawColor(1);
    u8g2.drawStr(5, 33, "abcd");
    u8g2.setDrawColor(2);
    u8g2.drawStr(5, 48, "abcd");
    */
    if (box_num == 1){
        strip_length_left = rotaryEncoder.readEncoder();
        oled.drawBox(0,15,41,20);
    }
    if (box_num == 2){
        wire_length = rotaryEncoder.readEncoder();
        oled.drawBox(42,15,41,20);
    }
    if (box_num == 3){
        strip_length_right = rotaryEncoder.readEncoder();
        oled.drawBox(85,15,41,20);
    }
    if (box_num == 4){
        quantity = rotaryEncoder.readEncoder();
        oled.drawBox(0,40,41,20);
    }
    if (box_num == 5){
        strip_depth = rotaryEncoder.readEncoder();
        oled.drawBox(42,40,41,20);
    }  
    if (box_num == 6){
        oled.drawBox(85,40,41,20);
    }
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
  if (button_mode == ADJUST_VALUE) {
    
    if (read_encoder_value_mode()) {
      oled_update();
    }
  }
  else {
    //button_mode == SELECT_BOXES
    if (read_encoder_box_mode()) {
      oled_update();
    }
  }
}