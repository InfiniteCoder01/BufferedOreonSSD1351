#include "OreonBSSD1351.hpp"

void OreonBSSD1351::writeCmd(uint8_t c) {
#if defined(SPI_HAS_TRANSACTION)
  SPI.beginTransaction(spiSettings);
#endif
  digitalWrite(dc, LOW);
  digitalWrite(cs, LOW);
  SPI.write(c);
  digitalWrite(cs, HIGH);
#if defined(SPI_HAS_TRANSACTION)
  SPI.endTransaction();
#endif
}

void OreonBSSD1351::writeData(uint8_t c) {
#if defined(SPI_HAS_TRANSACTION)
  SPI.beginTransaction(spiSettings);
#endif
  digitalWrite(dc, HIGH);
  digitalWrite(cs, LOW);
  SPI.write(c);
  digitalWrite(cs, HIGH);
#if defined(SPI_HAS_TRANSACTION)
  SPI.endTransaction();
#endif
}

void OreonBSSD1351::setAddressWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  writeCmd(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(x + w - 1);
  writeCmd(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(y + h - 1);
  writeCmd(SSD1351_CMD_WRITERAM);
#if defined(SPI_HAS_TRANSACTION)
  SPI.beginTransaction(spiSettings);
#endif
  digitalWrite(dc, HIGH);
  digitalWrite(cs, LOW);
}

void OreonBSSD1351::endWrite() {
  digitalWrite(cs, HIGH);
#if defined(SPI_HAS_TRANSACTION)
  SPI.endTransaction();
#endif
}

void OreonBSSD1351::update() {
  setAddressWindow(0, 0, width, height);
  SPI.writeBytes((uint8_t*)bufferW, width * height * 2);
  endWrite();
}

void OreonBSSD1351::update(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  setAddressWindow(x, y, w, h);
  for (uint8_t y1 = y; y1 < y + h; y1++) {
    SPI.writeBytes((uint8_t*)&bufferW[y1 * width], w * 2);
  }
  endWrite();
}

void OreonBSSD1351::begin(int _cs, int _dc, int _rst) {
  cs = _cs;
  dc = _dc;
  rst = _rst;
  pinMode(cs, OUTPUT);
  pinMode(dc, OUTPUT);
  pinMode(rst, OUTPUT);

  SPI.begin();
  // SPI.setFrequency(SPI_SPEED);
  // SPI.setBitOrder(MSBFIRST);
  digitalWrite(cs, LOW);

  if (_rst != -1) {
    digitalWrite(rst, HIGH);
    delay(10);
    digitalWrite(rst, LOW);
    delay(10);
    digitalWrite(rst, HIGH);
    delay(10);
  }

  writeCmd(0xFD);   // Set Command Lock
  writeData(0x12);  // Unlock OLED driver IC MCU interface from entering command
  writeCmd(0xFD);   // Set Command Lock
  writeData(0xB1);  // Command A2,B1,B3,BB,BE,C1 accessible if in unlock state
  writeCmd(0xAE);   // Sleep mode On (Display OFF)
  writeCmd(0xB3);   // Front Clock Divider
  writeCmd(0xF1);   // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
  writeCmd(0xCA);   // Set MUX Ratio
  writeData(127);
  writeCmd(0xA0);        // Set Re-map
  writeData(0b01110100);  // 65k color
  // writeData(0b10110100); //262k color
  // writeData(0b11110100); //262k color, 16-bit format 2
  writeCmd(0x15);     // Set Column
  writeData(0);       // start
  writeData(width);   // end
  writeCmd(0x75);     // Set Row
  writeData(0);       // start
  writeData(height);  // end
  writeCmd(0xA1);     // Set Display Start Line
  writeData(0);
  writeCmd(0xA2);  // Set Display Offset
  writeData(0);
  writeCmd(0xB5);  // Set GPIO
  writeData(0);
  writeCmd(0xAB);   // Function Selection
  writeData(0x01);  // Enable internal Vdd /8-bit parallel
  // writeData(0b01000001); //Enable internal Vdd /Select 16-bit parallel interface
  writeCmd(0xB1);  // Set Reset(Phase 1) /Pre-charge(Phase 2)
  // writeCmd(0b00110010); //5 DCLKs / 3 DCLKs
  writeCmd(0x74);
  writeCmd(0xBE);   // Set VCOMH Voltage
  writeCmd(0x05);   // 0.82 x VCC [reset]
  writeCmd(0xA6);   // Reset to normal display
  writeCmd(0xC1);   // Set Contrast
  writeData(0xC8);  // Red contrast (reset=0x8A)
  writeData(0x80);  // Green contrast (reset=0x51)
  writeData(0xC8);  // Blue contrast (reset=0x8A)
  writeCmd(0xC7);   // Master Contrast Current Control
  writeData(0x0F);  // 0-15
  writeCmd(0xB4);   // Set Segment Low Voltage(VSL)
  writeData(0xA0);
  writeData(0xB5);
  writeData(0x55);
  writeCmd(0xB6);   // Set Second Precharge Period
  writeData(0x01);  // 1 DCLKS
  writeCmd(0x9E);   // Scroll Stop Moving
  writeCmd(0xAF);   // Sleep mode On (Display ON)

  clear();
  update();
}

/********** SHAPES **********/
void OreonBSSD1351::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
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

void OreonBSSD1351::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color, uint8_t thickness) {
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

/********** IMAGES **********/
void OreonBSSD1351::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t bitmap[], int32_t color, int32_t background, float scale, bool yx) {
  if (x >= width || y >= height || x < -w * scale || y < -h * scale) return;
  if (color == -1) color = textColor;
  if (background == -1) background = backgroundColor;
  uint8_t _byte;
  int position = -1;
  for (uint16_t x1 = max(-x, 0); x1 < min((float)width, w * scale); x1++) {
    for (uint16_t y1 = max(-y, 0); y1 < min((float)height, h * scale); y1++) {
      uint16_t index = min((int)round(x1 / scale), w - 1) + min((int)round(y1 / scale), h - 1) * Math::alignUp(w, 8);
      if (yx) index = min((int)round(y1 / scale), h - 1) + min((int)round(x1 / scale), w - 1) * Math::alignUp(h, 8);
      if (index / 8 != position) _byte = pgm_read_byte(&bitmap[index / 8]);
      if (bitRead(_byte, 7 - index % 8)) {
        _setPixel(x + (flip ? w * scale - x1 - 1 : x1), y + (yx ? h * scale - y1 - 1 : y1), color);
      } else if (background != color) {
        _setPixel(x + (flip ? w * scale - x1 - 1 : x1), y + (yx ? h * scale - y1 - 1 : y1), background);
      }
    }
  }
}

void OreonBSSD1351::fastDrawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[]) {
  if (x >= width || y >= height || x < -w || y < -h) return;
  int16_t x1 = max(0, -x);
  for (uint8_t y1 = max(0, -y); y1 < min((int)h, height - y); y1++) {
    memcpy_P(buffer + ((y + y1) * width + x + x1) * 2, image + (y1 * w + x1) * 2, min((int)(w - x1), width - x) * 2);
  }
}

void OreonBSSD1351::drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[], int32_t alphaColor, uint8_t skip) {
  if (x >= width || y >= height || x < -w || y < -h || w < 0 || h < 0) return;
  for (uint8_t x1 = max(-x, 0); x1 < min((int)w, width - x); x1++) {
    for (uint8_t y1 = max(-y, 0); y1 < min((int)h, height - y); y1++) {
      uint16_t index = (flip ? w - x1 - 1 : x1) + y1 * (w + skip);
      uint16_t color = (pgm_read_byte(&image[index * 2]) << 8) | pgm_read_byte(&image[index * 2 + 1]);
      if (color != alphaColor) _setPixel(x + x1, y + y1, color);
    }
  }
}

/********** TEXT **********/
size_t OreonBSSD1351::write(uint8_t c) {
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
  return 1;
}
