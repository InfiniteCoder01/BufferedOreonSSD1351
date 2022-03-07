#include <OreonBSSD1351.hpp>
#include "images.hpp"

void setup()  {
  oled::begin();
  delay(1000);
  oled::fillScreen(YELLOW);
  oled::update();
  for (int i = 0; i < 10; i++) {
    oled::fillRect(random(128), random(128), random(128), random(128), random(0xffff));
    oled::update();
    delay(100);
  }
  delay(1000);
  oled::fillScreen(BLACK);
  oled::drawBitmap(0, 0, 16, 16, bmp, RED, WHITE); // bitmap and background color's are optional
  oled::drawImage(16, 0, 16, 16, img);
	oled::update();
}

void loop() {
}