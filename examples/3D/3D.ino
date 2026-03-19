#define SPI_SPEED 39999999

#include <OreonBSSD1351.hpp>
#include <OreonBSSD13513D.hpp>
#include <OreonBSSD1351Gui.hpp>

void setup()  {
  Serial.begin(115200);
  oled::begin(5, 17, 16);
  o3d::begin();
}

void loop() {
  oled::clear();
  o3d::clear();
  o3d::draw();
  gui::drawFPS();
  oled::update();
}
