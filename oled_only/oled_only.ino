#include <Arduino.h>
#include <U8g2lib.h>

// Specify which pins are used on the microcontroller to communicate with the OLED display
#define OLED_CS D8 // clock
#define OLED_RES D3 // reset
//#define OLED_DC D4 // data
#define OLED_DC D0 // data

// Constructor - This initializes "oled" so now you can use it to call functions (e.g., oled.drawStr)
// See this link for a list of all the functions that can be used
// https://github.com/olikraus/u8g2/wiki/u8g2reference
U8G2_SSD1309_128X64_NONAME2_1_4W_HW_SPI oled(U8G2_R0, OLED_CS, OLED_DC, OLED_RES);  

// Variables for drawing the wire at the top of the OLED screen.
u8g2_uint_t SCREEN_WIDTH = 0;
u8g2_uint_t SCREEN_HEIGHT = 0;
const int WIRE_STRAND_LENGTH = 30;
const int WIRE_STRAND_Y_POS = 7;
const int WIRE_INSULATION_WIDTH = SCREEN_WIDTH - (WIRE_STRAND_LENGTH * 2);
const int WIRE_INSULATION_HEIGHT = 14;

// The setup function will execute once on the microcontroller right after boot-up or on reset
void setup() {

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

// This will be part of your task - to create a function that draws a wire image across the top of the screen
//        ______
//   -----|    |-----
void drawWire() {
    //oled.drawLine(0, WIRE_STRAND_Y_POS, WIRE_STRAND_LENGTH, WIRE_STRAND_Y_POS, SH110X_WHITE);
    //oled.fillRect(WIRE_STRAND_LENGTH, 0, WIRE_INSULATION_WIDTH, WIRE_INSULATION_HEIGHT, SH110X_WHITE);
    //oled.drawLine(WIRE_STRAND_LENGTH + WIRE_INSULATION_WIDTH, WIRE_STRAND_Y_POS, SCREEN_WIDTH, WIRE_STRAND_Y_POS, SH110X_WHITE);
}

// Microcontroller will loop indefinitely
void loop() {

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
    //oled.setCursor(0, 0);
    //oled.print("0");

    // This will be part of your first task...
    //oled.drawWire();


  } while (oled.nextPage());

  delay(1000);
}
