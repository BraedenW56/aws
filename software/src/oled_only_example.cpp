#include <Arduino.h>
#include <U8g2lib.h>

#define OLED_CS D8
#define OLED_RES D3
//#define OLED_DC D4
#define OLED_DC D0

U8G2_SSD1309_128X64_NONAME2_1_4W_HW_SPI oled(U8G2_R1, OLED_CS, OLED_DC, OLED_RES);  

u8g2_uint_t oledWidth = 0;
u8g2_uint_t oledHeight = 0;

bool led = false;
u8g2_uint_t x = 0;
u8g2_uint_t y = 0;

void setup() {
  // Initialize the Serial terminal
  Serial.begin(115200);
  Serial.println("Starting up...");

  // Initialize the OLED display
  oled.begin();
  oled.setFont(u8g2_font_logisoso16_tf);
  oled.setDisplayRotation(U8G2_R0); 
  Serial.println("OLED initialized");

  // Precompute some variables - No point in doing these things in the "loop" section 
  // since the results would never change
  // Get the actual display width and height so we can compute pixel locations later
  oledHeight = oled.getDisplayHeight();
  oledWidth = oled.getDisplayWidth();
}

void loop() {

  Serial.print("Value: ");
    
  // Draw stuff on the OLED
  oled.firstPage();
  do {
    oled.drawStr(0, 20, "HELLO WORLD");
  } while (oled.nextPage());

  delay(1000);
}
