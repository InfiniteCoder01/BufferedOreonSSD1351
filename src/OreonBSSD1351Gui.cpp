#include "OreonBSSD1351Gui.hpp"

namespace gui {
void darkenRect(OreonBSSD1351 &oled, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t alpha) {
  if (x >= oled.getWidth() || y >= oled.getHeight() || x <= -w || y <= -h) return;

  if (x < 0) w += x, x = 0;
  if (y < 0) h += y, y = 0;
  if (x + w > oled.getWidth()) w = oled.getWidth() - x;
  if (y + h > oled.getHeight()) h = oled.getHeight() - y;

  for (uint8_t x1 = x; x1 < x + w; x1++) {
    for (uint8_t y1 = y; y1 < y + h; y1++) {
      oled._setPixel(x1, y1, darkenColor(oled._getPixel(x1, y1), alpha));
    }
  }
}

void type(OreonBSSD1351 &oled, String text, uint16_t typeDelay) {
  for (int i = 0; i < text.length(); i++) {
    uint32_t t = millis();
    oled.write(text[i]);
    oled.update();
    while (millis() - t < typeDelay) yield();
  }
}

void textAt(OreonBSSD1351 &oled, String text, int x, int y, TextAlignment horizontal, TextAlignment vertical) {
  uint16_t maxWidth = 0, totalHeight = -1;
  for (int l = 0, r = text.indexOf('\n', l); l < text.length(); l = r + 1, r = text.indexOf('\n', l)) {
    if (r == -1) r = text.length();
    maxWidth = Math::max(maxWidth, oled.getStringWidth(text.substring(l, r)));
    totalHeight += oled.getCharHeight() + 1;
  }

  x -= maxWidth * (horizontal + 1) / 2;
  y -= maxWidth * (vertical + 1) / 2;
  for (int l = 0, r = text.indexOf('\n', l); l < text.length(); l = r + 1, r = text.indexOf('\n', l)) {
    if (r == -1) r = text.length();
    oled.setCursor(x, y);
    oled.print(text.substring(l, r));
    y += oled.getCharHeight() + 1;
  }
}

void drawList(OreonBSSD1351 &oled, String name, String* elements, uint8_t elementCount, uint8_t pointer) {
  centerText(oled, name, 0);
  for (int i = 0; i < elementCount; i++) {
    if (i == pointer) {
      oled.write('>');
    } else {
      oled.write(' ');
    }
    oled.println(elements[i]);
  }
}

void drawFPS(OreonBSSD1351 &oled) {
  static uint32_t t;
  static uint16_t prev;
  oled.setCursor(1, 1);
  uint16_t dt = millis() - t;
  t = millis();
  uint16_t approx = (prev + dt) / 2;
  prev = dt;
  oled.println(("FPS: " + String(1000.0 / approx, 1)).c_str());
}
}  // namespace gui
