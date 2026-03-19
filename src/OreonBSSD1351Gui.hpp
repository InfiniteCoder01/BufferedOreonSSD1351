#pragma once
#include "OreonBSSD1351.hpp"

namespace gui {
enum TextAlignment : int8_t {
  LEFT = -1,
  TOP = -1,
  CENTER = 0,
  RIGHT = 1,
  BOTTOM = 1,
};

void darkenRect(OreonBSSD1351 &oled, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t alpha);
void type(OreonBSSD1351 &oled, const String &text, uint16_t typeDelay = 100);
inline String typeAsync(const String &text, uint32_t typeStart, uint16_t typeDelay = 100) {
  return text.substring(0, (millis() - typeStart) / typeDelay);
}

void textAt(OreonBSSD1351 &oled, const String &text, int x, int y, TextAlignment horizontal = LEFT, TextAlignment vertical = TOP);
inline void textAt(OreonBSSD1351 &oled, const String &text, VectorMath::vec2i pos, TextAlignment horizontal = LEFT, TextAlignment vertical = TOP) {
  textAt(oled, text, pos.x, pos.y, horizontal, vertical);
}

inline void centerText(OreonBSSD1351 &oled, const String &text, int y = -1) {
  textAt(oled, text, oled.getWidth() / 2, y == -1 ? oled.getHeight() / 2 : y, CENTER, y == -1 ? CENTER : TOP);
}

inline void rightText(OreonBSSD1351 &oled, const String &text, int y = 0) {
  textAt(oled, text, oled.getWidth(), y, RIGHT);
}

void drawList(OreonBSSD1351 &oled, const String &name, const String *elements, uint8_t elementCount, uint8_t pointer);
void drawFPS(OreonBSSD1351 &oled);
}  // namespace gui
