#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <fonts/fonts.hpp>
#include <OreonMath.hpp>

#ifdef SPI_HAS_TRANSACTION
#ifndef SPI_SPEED
#if defined(ESP8266)
#define SPI_SPEED 7999999
#elif defined(ESP32)
#define SPI_SPEED 39999999
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

const uint8_t SSD1351_CMD_SETCOLUMN = 0x15;
const uint8_t SSD1351_CMD_SETROW = 0x75;
const uint8_t SSD1351_CMD_WRITERAM = 0x5C;
const uint8_t SSD1351_CMD_READRAM = 0x5D;
const uint8_t SSD1351_CMD_SETREMAP = 0xA0;
const uint8_t SSD1351_CMD_STARTLINE = 0xA1;
const uint8_t SSD1351_CMD_DISPLAYOFFSET = 0xA2;
const uint8_t SSD1351_CMD_DISPLAYALLOFF = 0xA4;
const uint8_t SSD1351_CMD_DISPLAYALLON = 0xA5;
const uint8_t SSD1351_CMD_NORMALDISPLAY = 0xA6;
const uint8_t SSD1351_CMD_INVERTDISPLAY = 0xA7;
const uint8_t SSD1351_CMD_FUNCTIONSELECT = 0xAB;
const uint8_t SSD1351_CMD_DISPLAYOFF = 0xAE;
const uint8_t SSD1351_CMD_DISPLAYON = 0xAF;
const uint8_t SSD1351_CMD_PRECHARGE = 0xB1;
const uint8_t SSD1351_CMD_DISPLAYENHANCE = 0xB2;
const uint8_t SSD1351_CMD_CLOCKDIV = 0xB3;
const uint8_t SSD1351_CMD_SETVSL = 0xB4;
const uint8_t SSD1351_CMD_SETGPIO = 0xB5;
const uint8_t SSD1351_CMD_PRECHARGE2 = 0xB6;
const uint8_t SSD1351_CMD_SETGRAY = 0xB8;
const uint8_t SSD1351_CMD_USELUT = 0xB9;
const uint8_t SSD1351_CMD_PRECHARGELEVEL = 0xBB;
const uint8_t SSD1351_CMD_VCOMH = 0xBE;
const uint8_t SSD1351_CMD_SCANINC = 0xC0;
const uint8_t SSD1351_CMD_CONTRASTABC = 0xC1;
const uint8_t SSD1351_CMD_CONTRASTMASTER = 0xC7;
const uint8_t SSD1351_CMD_MUXRATIO = 0xCA;
const uint8_t SSD1351_CMD_COMMANDLOCK = 0xFD;
const uint8_t SSD1351_CMD_HORIZSCROLL = 0x96;
const uint8_t SSD1351_CMD_STOPSCROLL = 0x9E;
const uint8_t SSD1351_CMD_STARTSCROLL = 0x9F;

const uint16_t BLACK = 0x0000;
const uint16_t GRAY = 0x73AE;
const uint16_t BLUE = 0x001F;
const uint16_t RED = 0xF800;
const uint16_t GREEN = 0x07E0;
const uint16_t CYAN = 0x07FF;
const uint16_t MAGENTA = 0xF81F;
const uint16_t YELLOW = 0xFFE0;
const uint16_t WHITE = 0xFFFF;

inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) { return r | (uint16_t)g << 5 | (uint16_t)b << 11; }
inline uint16_t rgb888to565(uint8_t r, uint8_t g, uint8_t b) { return rgb565(r >> 3, g >> 2, b >> 3); }
inline uint16_t interpolateColor(uint16_t color1, uint16_t color2, float t) {
  int r1 = (color1 & 31), g1 = (color1 >> 5 & 63), b1 = (color1 >> 11 & 31);
  int r2 = (color2 & 31), g2 = (color2 >> 5 & 63), b2 = (color2 >> 11 & 31);
  int r = Math::lerp(r1, r2, t);
  int g = Math::lerp(g1, g2, t);
  int b = Math::lerp(b1, b2, t);
  return rgb565(r, g, b);
}

inline uint16_t contrastColor(uint16_t color) {
  uint8_t r = (color & 31), g = (color >> 6 & 31), b = (color >> 11 & 31);
  uint8_t brightness = ((uint16_t)r + g + b) / 3;
  return brightness < 16 ? WHITE : BLACK;
}

inline uint16_t darkenColor(uint16_t color, uint8_t mul) {
  uint8_t r = (color & 31), g = (color >> 5 & 63), b = (color >> 11 & 31);
  r = (uint16_t)r * mul / 255;
  g = (uint16_t)g * mul / 255;
  b = (uint16_t)b * mul / 255;
  return rgb565(r, g, b);
}

class OreonBSSD1351 : public Print {
protected:
  uint8_t *buffer;
  uint8_t *bufferW;

  uint8_t fontW, fontH;
  const uint8_t *font;
  bool fontType;

  uint8_t width, height;
  bool trippleBuffer;

  int8_t cs, dc, rst;

public:
  int16_t cursorX = 0, cursorY = 0;
  float textSize = 1.0;
  uint16_t textColor = WHITE, backgroundColor = WHITE;

  bool flip = false; // Wether to flip all images
  bool wrap = true; // Wether to wrap text
  uint16_t pageX = 0, pageR; // Text wrap boundaries

#if defined(SPI_HAS_TRANSACTION)
  SPISettings spiSettings = SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0);
#endif

  OreonBSSD1351(uint8_t width = 128, uint8_t height = 128, bool trippleBuffer = false)
    : width(width), height(height), trippleBuffer(trippleBuffer) {
    buffer = new uint8_t[(size_t)width * height * 2];
    if (trippleBuffer) bufferW = new uint8_t[(size_t)width * height * 2];
    else bufferW = buffer;

    pageR = width;
    setFont(font8x12);
  }

  ~OreonBSSD1351() {
    delete[] buffer;
    if (trippleBuffer) delete[] bufferW;
  }

  uint16_t getWidth() const { return width; }
  uint16_t getHeight() const { return height; }
  
  VectorMath::vec2i size() const {
    return VectorMath::vec2i(width, height);
  }

  void writeCmd(uint8_t c);
  void writeData(uint8_t c);
  void setAddressWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  inline void setAddressWindow(VectorMath::vec2i pos, VectorMath::vec2i size) { setAddressWindow(pos.x, pos.y, size.x, size.y); }
  void endWrite();

  void update();
  void update(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  inline void update(VectorMath::vec2i pos, VectorMath::vec2i size) {
    update(pos.x, pos.y, size.x, size.y);
  }

  inline void swapBuffers() {
    if (trippleBuffer) Math::swap(buffer, bufferW);
  }

  void begin(int _cs, int _dc, int _rst);

  /********** PIXELS **********/
  void _setPixel(int16_t x, int16_t y, uint16_t color) {
    uint16_t index = y * width + x;
    buffer[index * 2] = color >> 8;
    buffer[index * 2 + 1] = color;
  }

  void setPixel(int16_t x, int16_t y, uint16_t color) {
    if (x >= width || y >= height || x < 0 || y < 0) return;
    _setPixel(x, y, color);
  }

  inline void setPixel(VectorMath::vec2i pos, uint16_t color) {
    setPixel(pos.x, pos.y, color);
  }

  uint16_t _getPixel(int16_t x, int16_t y) const {
    uint16_t index = y * width + x;
    return (uint16_t)buffer[index * 2] << 8 | buffer[index * 2 + 1];
  }

  uint16_t getPixel(int16_t x, int16_t y) const {
    if (x >= width || y >= height || x < 0 || y < 0) return BLACK;
    return _getPixel(x, y);
  }

  inline uint16_t getPixel(VectorMath::vec2i pos) {
    return getPixel(pos.x, pos.y);
  }

  void setPixelAlpha(int16_t x, int16_t y, uint16_t color, float alpha) {
    setPixel(x, y, interpolateColor(getPixel(x, y), color, alpha));
  }

  inline void setPixelAlpha(VectorMath::vec2i pos, uint16_t color, float alpha) {
    setPixelAlpha(pos.x, pos.y, color, alpha);
  }

  /********** SHAPES **********/
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  inline void fillRect(VectorMath::vec2i pos, VectorMath::vec2i size, uint16_t color) {
    fillRect(pos.x, pos.y, size.x, size.y, color);
  }

  void _drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    for (int i = 0; i < w; i++) _setPixel(x + i, y, color);
  }

  void _drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    for (int i = 0; i < h; i++) _setPixel(x, y + i, color);
  }

  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if (x + w <= 0 || x >= width || y < 0 || y >= height) return;
    if (x < 0) w += x, x = 0;
    if (x + w > width) w = width - x;
    _drawFastHLine(x, y, w, color);
  }

  inline void drawFastHLine(VectorMath::vec2i pos, int16_t w, uint16_t color) {
    drawFastHLine(pos.x, pos.y, w, color);
  }

  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    if (x < 0 || x >= width || y + h <= 0 || y >= height) return;
    if (y < 0) h += y, y = 0;
    if (y + h > height) h = height - y;
    _drawFastVLine(x, y, h, color);
  }

  inline void drawFastVLine(VectorMath::vec2i pos, int16_t h, uint16_t color) {
    drawFastVLine(pos.x, pos.y, h, color);
  }

  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color, uint8_t thickness = 1);
  inline void drawLine(VectorMath::vec2i pos0, VectorMath::vec2i pos1, uint16_t color, uint8_t thickness = 1) {
    drawLine(pos0.x, pos0.y, pos1.x, pos1.y, color, thickness);
  }

  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawFastVLine(x, y, h, color);
    drawFastHLine(x, y, w, color);
    drawFastVLine(x + w, y, h, color);
    drawFastHLine(x, y + h, w + 1, color);
  }

  inline void drawRect(VectorMath::vec2i pos, VectorMath::vec2i size, uint16_t color) {
    drawRect(pos.x, pos.y, size.x, size.y, color);
  }

  void fillScreen(uint16_t color) {
    for (size_t i = 0; i < (size_t)width * height; i++) {
      buffer[i * 2] = color >> 8;
      buffer[i * 2 + 1] = color;
    }
  }

  void clear() {
    if (backgroundColor == textColor) fillScreen(BLACK);
    else fillScreen(backgroundColor);
  }

  /********** IMAGES **********/
  void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t bitmap[], int32_t color = -1, int32_t background = -1, float scale = 1, bool yx = false);
  inline void drawBitmap(VectorMath::vec2i pos, VectorMath::vec2i size, const uint8_t bitmap[], int32_t color = -1, int32_t background = -1, float scale = 1) {
    drawBitmap(pos.x, pos.y, size.x, size.y, bitmap, color, background, scale);
  }

  void drawBackground(const uint8_t background[]) { memcpy_P(buffer, background, width * height * 2); }

  void fastDrawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[]);
  inline void fastDrawImage(VectorMath::vec2i pos, VectorMath::vec2i size, const uint8_t image[]) {
    fastDrawImage(pos.x, pos.y, size.x, size.y, image);
  }

  void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[], int32_t alphaColor = -1, uint8_t skip = 0);
  inline void drawImage(VectorMath::vec2i pos, VectorMath::vec2i size, const uint8_t image[], int32_t alphaColor = -1, uint8_t skip = 0) {
    drawImage(pos.x, pos.y, size.x, size.y, image, alphaColor, skip);
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

  /********** TEXT **********/
  void setFont(const uint8_t *font) {
    this->font = font + 3;
    fontW = pgm_read_byte(&font[0]);
    fontH = pgm_read_byte(&font[1]);
    fontType = pgm_read_byte(&font[2]);
  }

  const uint8_t *getFont() const {
    return font - 3;
  }

  void setCursor(uint16_t x, uint16_t y) {
    cursorX = x;
    cursorY = y;
  }

  inline void setCursor(VectorMath::vec2i pos) {
    setCursor(pos.x, pos.y);
  }

  inline VectorMath::vec2i getCursor() const {
    return VectorMath::vec2i(cursorX, cursorY);
  }

  void setTextColor(uint16_t color, uint16_t background) {
    textColor = color;
    backgroundColor = background;
  }

  void setTextColor(uint16_t color) {
    textColor = backgroundColor = color;
  }

  uint16_t getStringWidth(String s) { return s.length() * fontW * textSize; }
  uint16_t getCharHeight() { return fontH * textSize; }

  size_t write(uint8_t c) override;
};
