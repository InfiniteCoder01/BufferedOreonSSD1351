#pragma once
#include <stdint.h>
#include "OreonBSSD1351.hpp"

namespace gui {
OREON_BSSD_DECL void centerText(String text, int y = -1) {
  oled::setCursor(oled::width / 2 - oled::getStringWidth(text) / 2, (y == -1 ? (oled::height / 2 - oled::getCharHeight() / 2) : y));
  oled::println(text);
}

OREON_BSSD_DECL void type(String text, uint16_t typeDelay = 100) {
  for (int i = 0; i < text.length(); i++) {
    uint32_t t = millis();
    oled::write(text[i]);
    oled::update();
    while (millis() - t < typeDelay) yield();
  }
}

OREON_BSSD_DECL String typeAsync(String text, uint32_t typeStart, uint16_t typeDelay = 100) { return text.substring(0, (millis() - typeStart) / typeDelay); }

OREON_BSSD_DECL void textAt(String text, int x, int y) {
  while (true) {
    oled::setCursor(x, y);
    if (text.indexOf('\n') == -1) {
      oled::print(text);
      return;
    } else {
      oled::print(text.substring(0, text.indexOf('\n')));
      y += oled::getCharHeight() + 1;
      text = text.substring(text.indexOf('\n') + 1);
    }
  }
}

OREON_BSSD_DECL void rightText(String text, int y = 0) {
  oled::setCursor(oled::width - oled::getStringWidth(text), y);
  oled::print(text);
}

OREON_BSSD_DECL void drawList(String name, String* elements, uint8_t elementCount, uint8_t pointer) {
  centerText(name, 0);
  for (int i = 0; i < elementCount; i++) {
    if (i == pointer) {
      oled::write('>');
    } else {
      oled::write(' ');
    }
    oled::println(elements[i]);
  }
}

OREON_BSSD_DECL void drawFPS() {
  static uint32_t t;
  static uint16_t prev;
  oled::setCursor(1, 1);
  uint16_t dt = millis() - t;
  t = millis();
  uint16_t approx = (prev + dt) / 2;
  prev = dt;
  oled::println(("FPS: " + String(1000.0 / approx, 1)).c_str());
}

OREON_BSSD_DECL void textAt(String text, VectorMath::vec2i pos) { textAt(text, pos.x, pos.y); }
}  // namespace gui
