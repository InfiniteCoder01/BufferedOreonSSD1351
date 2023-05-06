#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <fonts/fonts.hpp>
#include <OreonMath.hpp>

#define SSD1351_CMD_SETCOLUMN 0x15
#define SSD1351_CMD_SETROW 0x75
#define SSD1351_CMD_WRITERAM 0x5C
#define SSD1351_CMD_READRAM 0x5D
#define SSD1351_CMD_SETREMAP 0xA0
#define SSD1351_CMD_STARTLINE 0xA1
#define SSD1351_CMD_DISPLAYOFFSET 0xA2
#define SSD1351_CMD_DISPLAYALLOFF 0xA4
#define SSD1351_CMD_DISPLAYALLON 0xA5
#define SSD1351_CMD_NORMALDISPLAY 0xA6
#define SSD1351_CMD_INVERTDISPLAY 0xA7
#define SSD1351_CMD_FUNCTIONSELECT 0xAB
#define SSD1351_CMD_DISPLAYOFF 0xAE
#define SSD1351_CMD_DISPLAYON 0xAF
#define SSD1351_CMD_PRECHARGE 0xB1
#define SSD1351_CMD_DISPLAYENHANCE 0xB2
#define SSD1351_CMD_CLOCKDIV 0xB3
#define SSD1351_CMD_SETVSL 0xB4
#define SSD1351_CMD_SETGPIO 0xB5
#define SSD1351_CMD_PRECHARGE2 0xB6
#define SSD1351_CMD_SETGRAY 0xB8
#define SSD1351_CMD_USELUT 0xB9
#define SSD1351_CMD_PRECHARGELEVEL 0xBB
#define SSD1351_CMD_VCOMH 0xBE
#define SSD1351_CMD_SCANINC 0xC0
#define SSD1351_CMD_CONTRASTABC 0xC1
#define SSD1351_CMD_CONTRASTMASTER 0xC7
#define SSD1351_CMD_MUXRATIO 0xCA
#define SSD1351_CMD_COMMANDLOCK 0xFD
#define SSD1351_CMD_HORIZSCROLL 0x96
#define SSD1351_CMD_STOPSCROLL 0x9E
#define SSD1351_CMD_STARTSCROLL 0x9F

#define BLACK 0x0000
#define GRAY 0x73AE
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#ifdef SPI_HAS_TRANSACTION
#ifndef SPI_SPEED
#if defined(ESP8266)
#define SPI_SPEED 7999999
#elif defined(__AVR__)
#define SPI_SPEED 8000000
#elif defined(__SAM3X8E__)
#define SPI_SPEED 24000000
#elif defined(__MKL26Z64__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define SPI_SPEED 30000000
#else
#define SPI_SPEED 8000000
#endif
#endif
#endif

#if 0
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE inline
#endif

#ifndef OREON_BSSD_DECL
#define OREON_BSSD_DECL static
#endif

namespace oled {
enum TextAlignment : int8_t { LEFT = -1, CENTER = 0, RIGHT = 1 };

/********** VARIABLES **********/
#if defined(SPI_HAS_TRANSACTION)
extern SPISettings _SPISettings;
#endif

extern uint8_t* buffer;
extern uint8_t* bufferW;

extern float textSize;
extern uint16_t textColor, backgroundColor;
extern uint8_t width, height, cursorX, cursorY, pageX, pageR, fontW, fontH;
extern const uint8_t* font;
extern bool fontType;
extern int8_t cs, dc, rst;
extern int8_t textAlignment;
extern bool flip, wrap;

/********** HELPERS **********/
inline VectorMath::vec2i size() { return VectorMath::vec2i(width, height); }
inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) { return (r >> 3) | (g >> 2 << 5) | (b >> 3 << 11); }
ALWAYS_INLINE uint16_t rgb565to565(uint8_t r, uint8_t g, uint8_t b) { return r | (g << 5) | (b << 11); }

OREON_BSSD_DECL uint16_t interpolateColor(uint16_t color1, uint16_t color2, float k) {
  uint8_t r1 = (color1 & 31), g1 = (color1 >> 5 & 63), b1 = (color1 >> 11 & 31);
  uint8_t r2 = (color2 & 31), g2 = (color2 >> 5 & 63), b2 = (color2 >> 11 & 31);
  uint8_t r = Math::lerp(r1, r2, k);
  uint8_t g = Math::lerp(g1, g2, k);
  uint8_t b = Math::lerp(b1, b2, k);
  return rgb565to565(r, g, b);
}

OREON_BSSD_DECL uint16_t contrastColor(uint16_t color) {
  uint8_t r = (color & 31), g = (color >> 6 & 31), b = (color >> 11 & 31);
  uint8_t brightness = ((uint16_t)r + g + b) / 3;
  return brightness < 16 ? WHITE : BLACK;
}

OREON_BSSD_DECL uint16_t darkenColor(uint16_t color, uint8_t alpha) {
  uint8_t r = (color & 31), g = (color >> 5 & 63), b = (color >> 11 & 31);
  r = (uint16_t)r * alpha / 255;
  g = (uint16_t)g * alpha / 255;
  b = (uint16_t)b * alpha / 255;
  return rgb565to565(r, g, b);
}

ALWAYS_INLINE void writeSPI(uint8_t c) { SPI.write(c); }
ALWAYS_INLINE void writeSPI16(uint16_t c) { SPI.write16(c); }
ALWAYS_INLINE void writeSPI32(uint32_t c) { SPI.write32(c); }

OREON_BSSD_DECL void writeCmd(uint8_t c) {
#if defined(SPI_HAS_TRANSACTION)
  SPI.beginTransaction(_SPISettings);
#endif
  digitalWrite(dc, LOW);
  digitalWrite(cs, LOW);
  writeSPI(c);
  digitalWrite(cs, HIGH);
#if defined(SPI_HAS_TRANSACTION)
  SPI.endTransaction();
#endif
}

OREON_BSSD_DECL void writeData(uint8_t c) {
#if defined(SPI_HAS_TRANSACTION)
  SPI.beginTransaction(_SPISettings);
#endif
  digitalWrite(dc, HIGH);
  digitalWrite(cs, LOW);
  writeSPI(c);
  digitalWrite(cs, HIGH);
#if defined(SPI_HAS_TRANSACTION)
  SPI.endTransaction();
#endif
}

OREON_BSSD_DECL void setAddressWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  writeCmd(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(x + w - 1);
  writeCmd(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(y + h - 1);
  writeCmd(SSD1351_CMD_WRITERAM);
#if defined(SPI_HAS_TRANSACTION)
  SPI.beginTransaction(_SPISettings);
#endif
  digitalWrite(dc, HIGH);
  digitalWrite(cs, LOW);
}

ALWAYS_INLINE void endWrite() {
  digitalWrite(cs, HIGH);
#if defined(SPI_HAS_TRANSACTION)
  SPI.endTransaction();
#endif
}

ALWAYS_INLINE void _setPixel(int16_t x, int16_t y, uint16_t color) {
  uint16_t index = y * width + x;
  buffer[index * 2] = color >> 8;
  buffer[index * 2 + 1] = color;
}

OREON_BSSD_DECL void setPixel(int16_t x, int16_t y, uint16_t color) {
  if (x >= width || y >= height || x < 0 || y < 0) return;
  _setPixel(x, y, color);
}

ALWAYS_INLINE uint16_t _getPixel(int16_t x, int16_t y) {
  uint16_t index = y * width + x;
  return buffer[index * 2] << 8 | buffer[index * 2 + 1];
}

OREON_BSSD_DECL uint16_t getPixel(int16_t x, int16_t y) {
  if (x >= width || y >= height || x < 0 || y < 0) return 0;
  return _getPixel(x, y);
}

OREON_BSSD_DECL void setPixelAlpha(int16_t x, int16_t y, uint16_t color, float alpha) { setPixel(x, y, interpolateColor(getPixel(x, y), color, alpha)); }

/********** DRAWMENT **********/
OREON_BSSD_DECL void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (x >= width || y >= height || x <= -w || y <= -h) return;

  if (x < 0) w += x, x = 0;
  if (y < 0) h += y, y = 0;
  if (x + w > width) w = width - x;
  if (y + h > height) h = height - y;

  for (uint8_t x1 = x; x1 < x + w; x1++) {
    for (uint8_t y1 = y; y1 < y + h; y1++) {
      _setPixel(x1, y1, color);
    }
  }
}

ALWAYS_INLINE void _drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  for (int i = 0; i < w; i++) _setPixel(x + i, y, color);
}

ALWAYS_INLINE void _drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  for (int i = 0; i < h; i++) _setPixel(x, y + i, color);
}

OREON_BSSD_DECL void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  if (x + w <= 0 || x >= width || y < 0 || y >= height) return;
  if (x < 0) w += x, x = 0;
  if (x + w > width) w = width - x;
  _drawFastHLine(x, y, w, color);
}

OREON_BSSD_DECL void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  if (x < 0 || x >= width || y + h <= 0 || y >= height) return;
  if (y < 0) h += y, y = 0;
  if (y + h > height) h = height - y;
  _drawFastVLine(x, y, h, color);
}

OREON_BSSD_DECL void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color, uint8_t thickness = 1) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    Math::swap(x0, y0);
    Math::swap(x1, y1);
  }

  if (x0 > x1) {
    Math::swap(x0, x1);
    Math::swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      fillRect(y0 - thickness / 2, x0 - thickness / 2, thickness, thickness, color);
    } else {
      fillRect(x0 - thickness / 2, y0 - thickness / 2, thickness, thickness, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

OREON_BSSD_DECL void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (x >= width || y >= height) return;
  if (x + w < 0 || y + h < 0) return;
  if (w < 0 || h < 0) return;
  drawFastVLine(x, y, h, color);
  drawFastHLine(x, y, w, color);
  drawFastVLine(x + w, y, h, color);
  drawFastHLine(x, y + h, w + 1, color);
}

inline void fillScreen(uint16_t color) { fillRect(0, 0, width, height, color); }

OREON_BSSD_DECL void clear() {
  if (backgroundColor == textColor) fillScreen(BLACK);
  else fillScreen(backgroundColor);
}

OREON_BSSD_DECL void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t bitmap[], uint16_t color = textColor, uint16_t background = backgroundColor, float scale = 1, bool yx = false) {
  if (x >= width || y >= height || x < -w * scale || y < -h * scale) return;
  uint8_t _byte;
  int position = -1;
  for (uint16_t x1 = max(-x, 0); x1 < min((float)width, w * scale); x1++) {
    for (uint16_t y1 = max(-y, 0); y1 < min((float)height, h * scale); y1++) {
      uint16_t index = min((int)round(x1 / scale), w - 1) + min((int)round(y1 / scale), h - 1) * Math::alignUp(w, 8);
      if (yx) index = min((int)round(y1 / scale), h - 1) + min((int)round(x1 / scale), w - 1) * Math::alignUp(h, 8);
      if (index / 8 != position) _byte = pgm_read_byte(&bitmap[index / 8]);
      uint8_t u = x + (flip ? w * scale - x1 - 1 : x1), v = y + (yx ? h * scale - y1 - 1 : y1);
      if (bitRead(_byte, 7 - index % 8)) {
        _setPixel(u, v, color == MAGENTA ? contrastColor(_getPixel(u, v)) : color);
      } else if (background != color) {
        _setPixel(u, v, background);
      }
    }
  }
}

OREON_BSSD_DECL void drawBackground(const uint8_t background[]) { memcpy_P(buffer, background, width * height * 2); }

OREON_BSSD_DECL void fastDrawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[]) {
  if (x >= width || y >= height || x < -w || y < -h) return;
  int16_t x1 = max(0, -x);
  for (uint8_t y1 = max(0, -y); y1 < min((int)h, height - y); y1++) {
    memcpy_P(buffer + ((y + y1) * width + x + x1) * 2, image + (y1 * w + x1) * 2, min((int)(w - x1), width - x) * 2);
  }
}

OREON_BSSD_DECL void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[], int32_t alphaColor = -1, uint8_t skip = 0) {
  if (x >= width || y >= height || x < -w || y < -h || w < 0 || h < 0) return;
  for (uint8_t x1 = max(-x, 0); x1 < min((int)w, width - x); x1++) {
    for (uint8_t y1 = max(-y, 0); y1 < min((int)h, height - y); y1++) {
      uint16_t index = (flip ? w - x1 - 1 : x1) + y1 * (w + skip);
      uint16_t color = (pgm_read_byte(&image[index * 2]) << 8) | pgm_read_byte(&image[index * 2 + 1]);
      if (color != alphaColor) _setPixel(x + x1, y + y1, color);
    }
  }
}

// void drawImageCompressed(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[], uint16_t offset = 0, int32_t alphaColor = -1) {
// 	uint8_t colorCount = pgm_read_byte(&image[0]);
// 	uint8_t bitCount = pgm_read_byte(&image[1]);
// 	const uint8_t* colors = image + 2;
// 	const uint8_t* bmp = image + colorCount * 2 + 2;
// 	uint16_t bytes, bytesIndex = 65535;

// 	for (uint8_t x1 = 0; x1 < w; x1++) {
// 		for (uint8_t y1 = 0; y1 < h; y1++) {
// 			uint16_t index = (y1 + x1 * h + offset) * bitCount;
// 			if (index / 8 != bytesIndex) {
// 				if (index / 8 + 1 == bytesIndex) {
// 					bytes <<= 8;
// 					bytes |= pgm_read_byte(&bmp[index / 8 + 1]);
// 				} else {
// 					bytes = pgm_read_byte(&bmp[index / 8]);
// 					bytes <<= 8;
// 					bytes |= pgm_read_byte(&bmp[index / 8 + 1]);
// 				}
// 			}
// 			uint8_t colorIndex = (bytes << index % 8) >> (16 - bitCount);
// 			uint16_t color = pgm_read_byte(&colors[colorIndex * 2]) << 8 | pgm_read_byte(&colors[colorIndex * 2 + 1]);
// 			if(color != alphaColor) setPixel(x + x1, y + y1, color);
// 		}
// 	}
// }

inline void setTextSize(float size) { textSize = size; }

OREON_BSSD_DECL void setFont(const uint8_t* font) {
  oled::font = font + 3;
  fontW = pgm_read_byte(&font[0]);
  fontH = pgm_read_byte(&font[1]);
  fontType = pgm_read_byte(&font[2]);
}

inline VectorMath::vec2u getCursor() { return {cursorX, cursorY}; }
inline void setCursor(uint8_t x, uint8_t y) {
  cursorX = x;
  cursorY = y;
}

inline void setTextColor(uint16_t color, uint16_t background) {
  textColor = color;
  backgroundColor = background;
}

inline void setTextColor(uint16_t color) { textColor = backgroundColor = color; }

inline uint8_t getStringWidth(String s) { return s.length() * fontW * textSize; }
inline uint8_t getCharHeight() { return fontH * textSize; }

OREON_BSSD_DECL void write(uint8_t c) {
  if (c > 31 && c < 131) {
    int fontWBits = (fontType == 1 ? fontW : Math::alignUp(fontW, 8)), fontHBits = (fontType == 1 ? Math::alignUp(fontH, 8) : fontH);
    drawBitmap(cursorX + pageX, cursorY, fontWBits, fontHBits, &font[int(c - ' ') * (fontWBits + (fontType == 1)) * fontHBits / 8], textColor, backgroundColor, textSize, fontType == 1);
  }
  cursorX += fontW * textSize;
  if (c == '\r') cursorX = 0;
  else if (c == '\n' || ((cursorX + pageX > pageR - fontW * textSize) && wrap)) {
    cursorX = 0;
    cursorY += fontH * textSize + 1;
  }
}

OREON_BSSD_DECL void print(String s) {
  // TODO: textAlignment
  for (char* c = s.begin(); c != s.end(); c++) write(*c);
}

OREON_BSSD_DECL void println(String s) { print(s + '\n'); }

OREON_BSSD_DECL void update(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  setAddressWindow(x, y, w, h);
  for (uint8_t y1 = y; y1 < y + h; y1++) {
    for (uint8_t x1 = x; x1 < x + w; x1++) {
      uint32_t index = y1 * width + x1;
      if ((x + w) - x1 > 1) {
        writeSPI32(bufferW[index * 2] << 24 | bufferW[index * 2 + 1] << 16 | bufferW[index * 2 + 2] << 8 | bufferW[index * 2 + 3]);
        x1++;
      } else {
        writeSPI16(bufferW[index * 2] << 8 | bufferW[index * 2 + 1]);
      }
    }
  }
  endWrite();
}

inline void update() { update(0, 0, width, height); }

OREON_BSSD_DECL void begin(int _cs, int _dc, int _rst) {
  cs = _cs;
  dc = _dc;
  rst = _rst;
  pinMode(cs, OUTPUT);
  pinMode(dc, OUTPUT);
  pinMode(rst, OUTPUT);

  SPI.begin();
  // SPI.setFrequency(SPI_SPEED);
  // SPI.setBitOrder(MSBFIRST);
  _SPISettings = SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0);
  digitalWrite(cs, LOW);

  if (_rst != -1) {
    digitalWrite(rst, HIGH);
    delay(10);
    digitalWrite(rst, LOW);
    delay(10);
    digitalWrite(rst, HIGH);
    delay(10);
  }

  writeCmd(0xFD);  // Set Command Lock
  writeData(0x12); // Unlock OLED driver IC MCU interface from entering command
  writeCmd(0xFD);  // Set Command Lock
  writeData(0xB1); // Command A2,B1,B3,BB,BE,C1 accessible if in unlock state
  writeCmd(0xAE);  // Sleep mode On (Display OFF)
  writeCmd(0xB3);  // Front Clock Divider
  writeCmd(0xF1);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
  writeCmd(0xCA);  // Set MUX Ratio
  writeData(127);
  writeCmd(0xA0);       // Set Re-map
  writeData(B01110100); // 65k color
  // writeData(B10110100); //262k color
  // writeData(B11110100); //262k color, 16-bit format 2
  writeCmd(0x15);    // Set Column
  writeData(0);      // start
  writeData(width);  // end
  writeCmd(0x75);    // Set Row
  writeData(0);      // start
  writeData(height); // end
  writeCmd(0xA1);    // Set Display Start Line
  writeData(0);
  writeCmd(0xA2); // Set Display Offset
  writeData(0);
  writeCmd(0xB5); // Set GPIO
  writeData(0);
  writeCmd(0xAB);  // Function Selection
  writeData(0x01); // Enable internal Vdd /8-bit parallel
  // writeData(B01000001); //Enable internal Vdd /Select 16-bit parallel interface
  writeCmd(0xB1); // Set Reset(Phase 1) /Pre-charge(Phase 2)
  // writeCmd(B00110010); //5 DCLKs / 3 DCLKs
  writeCmd(0x74);
  writeCmd(0xBE);  // Set VCOMH Voltage
  writeCmd(0x05);  // 0.82 x VCC [reset]
  writeCmd(0xA6);  // Reset to normal display
  writeCmd(0xC1);  // Set Contrast
  writeData(0xC8); // Red contrast (reset=0x8A)
  writeData(0x80); // Green contrast (reset=0x51)
  writeData(0xC8); // Blue contrast (reset=0x8A)
  writeCmd(0xC7);  // Master Contrast Current Control
  writeData(0x0F); // 0-15
  writeCmd(0xB4);  // Set Segment Low Voltage(VSL)
  writeData(0xA0);
  writeData(0xB5);
  writeData(0x55);
  writeCmd(0xB6);  // Set Second Precharge Period
  writeData(0x01); // 1 DCLKS
  writeCmd(0x9E);  // Scroll Stop Moving
  writeCmd(0xAF);  // Sleep mode On (Display ON)

  delay(100);
  setCursor(0, 0);
  setFont(font8x12);
  setTextSize(1);
  setTextColor(WHITE);
  flip = false;
  wrap = true;
  pageR = width;
#ifdef TRIPPLE_BUFFER
  bufferW = new uint8_t[width * height * 2];
#endif
  clear();
  update();
}

#ifdef TRIPPLE_BUFFER
inline void swapBuffers() { Math::swap(bufferW, buffer); }
#endif

ALWAYS_INLINE void setAddressWindow(VectorMath::vec2i pos, VectorMath::vec2i size) { setAddressWindow(pos.x, pos.y, size.x, size.y); }

ALWAYS_INLINE void setPixelAlpha(VectorMath::vec2i pos, uint16_t color, float alpha) { setPixelAlpha(pos.x, pos.y, color, alpha); }
ALWAYS_INLINE void setPixel(VectorMath::vec2i pos, uint16_t color) { oled::setPixel(pos.x, pos.y, color); }
ALWAYS_INLINE uint16_t getPixel(VectorMath::vec2i pos) { return oled::getPixel(pos.x, pos.y); }

ALWAYS_INLINE void fillRect(VectorMath::vec2i pos, VectorMath::vec2i size, uint16_t color) { fillRect(pos.x, pos.y, size.x, size.y, color); }
ALWAYS_INLINE void drawFastVLine(VectorMath::vec2i pos, int16_t h, uint16_t color) { drawFastVLine(pos.x, pos.y, h, color); }
ALWAYS_INLINE void drawFastHLine(VectorMath::vec2i pos, int16_t w, uint16_t color) { drawFastHLine(pos.x, pos.y, w, color); }
ALWAYS_INLINE void drawLine(VectorMath::vec2i pos0, VectorMath::vec2i pos1, uint16_t color, uint8_t thickness = 1) { drawLine(pos0.x, pos0.y, pos1.x, pos1.y, color, thickness); }
ALWAYS_INLINE void drawRect(VectorMath::vec2i pos, VectorMath::vec2i size, uint16_t color) { drawRect(pos.x, pos.y, size.x, size.y, color); }

ALWAYS_INLINE void drawBitmap(VectorMath::vec2i pos, VectorMath::vec2i size, const uint8_t bitmap[], uint16_t color = textColor, uint16_t background = backgroundColor, float scale = 1) { drawBitmap(pos.x, pos.y, size.x, size.y, bitmap, color, background, scale); }
ALWAYS_INLINE void fastDrawImage(VectorMath::vec2i pos, VectorMath::vec2i size, const uint8_t image[]) { fastDrawImage(pos.x, pos.y, size.x, size.y, image); }
ALWAYS_INLINE void drawImage(VectorMath::vec2i pos, VectorMath::vec2i size, const uint8_t image[], int32_t alphaColor = -1, uint8_t skip = 0) { drawImage(pos.x, pos.y, size.x, size.y, image, alphaColor, skip); }

ALWAYS_INLINE void setCursor(VectorMath::vec2i pos) { setCursor(pos.x, pos.y); }
ALWAYS_INLINE void update(VectorMath::vec2i pos, VectorMath::vec2i size) { update(pos.x, pos.y, size.x, size.y); }
} // namespace oled
