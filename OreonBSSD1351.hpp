#pragma once
#include <SPI.h>
#ifdef ESP8266
#include <LittleFS.h>
#endif
#include "fonts/fonts.hpp"

#define SSD1351_CMD_SETCOLUMN       0x15
#define SSD1351_CMD_SETROW          0x75
#define SSD1351_CMD_WRITERAM        0x5C
#define SSD1351_CMD_READRAM         0x5D
#define SSD1351_CMD_SETREMAP        0xA0
#define SSD1351_CMD_STARTLINE       0xA1
#define SSD1351_CMD_DISPLAYOFFSET   0xA2
#define SSD1351_CMD_DISPLAYALLOFF   0xA4
#define SSD1351_CMD_DISPLAYALLON    0xA5
#define SSD1351_CMD_NORMALDISPLAY   0xA6
#define SSD1351_CMD_INVERTDISPLAY   0xA7
#define SSD1351_CMD_FUNCTIONSELECT  0xAB
#define SSD1351_CMD_DISPLAYOFF      0xAE
#define SSD1351_CMD_DISPLAYON       0xAF
#define SSD1351_CMD_PRECHARGE       0xB1
#define SSD1351_CMD_DISPLAYENHANCE  0xB2
#define SSD1351_CMD_CLOCKDIV        0xB3
#define SSD1351_CMD_SETVSL          0xB4
#define SSD1351_CMD_SETGPIO         0xB5
#define SSD1351_CMD_PRECHARGE2      0xB6
#define SSD1351_CMD_SETGRAY         0xB8
#define SSD1351_CMD_USELUT          0xB9
#define SSD1351_CMD_PRECHARGELEVEL  0xBB
#define SSD1351_CMD_VCOMH           0xBE
#define SSD1351_CMD_SCANINC         0xC0
#define SSD1351_CMD_CONTRASTABC     0xC1
#define SSD1351_CMD_CONTRASTMASTER  0xC7
#define SSD1351_CMD_MUXRATIO        0xCA
#define SSD1351_CMD_COMMANDLOCK     0xFD
#define SSD1351_CMD_HORIZSCROLL     0x96
#define SSD1351_CMD_STOPSCROLL      0x9E
#define SSD1351_CMD_STARTSCROLL     0x9F

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

#define oreon_swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

namespace oled {
	uint16_t textColor, backgroundColor;
	uint8_t buffer[128 * 128 * 2];
	uint8_t width = 128, height = 128, cursorX, cursorY, fontW, fontH, brightness;
	const uint8_t* font;
	int cs = 15, dc = 4, rst = 5;
	bool flip;

	uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
		return uint16_t(r >> 3) | (g >> 2 << 5) | (b >> 3 << 11);
	}

	void writeSPI(uint8_t c) {
		SPI.transfer(c);
	}

	void writeCmd(uint8_t c) {
		digitalWrite(dc, LOW);
		digitalWrite(cs, LOW);
		writeSPI(c);
		digitalWrite(cs, HIGH);
	}

	void writeData(uint8_t c) {
		digitalWrite(dc, HIGH);
		digitalWrite(cs, LOW);
		writeSPI(c);
		digitalWrite(cs, HIGH);
	}

	void setAddressWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
		writeCmd(SSD1351_CMD_SETCOLUMN);
		writeData(x);
		writeData(x + w - 1);
		writeCmd(SSD1351_CMD_SETROW);
		writeData(y);
		writeData(y + h - 1);
		writeCmd(SSD1351_CMD_WRITERAM);
		digitalWrite(dc, HIGH);
		digitalWrite(cs, LOW);
	}

	inline void endWrite() {
		digitalWrite(cs, HIGH);
	}

	void setPixel(int16_t x, int16_t y, uint16_t color) {
		if (x >= width || y >= height || x < 0 || y < 0) return;
		uint16_t index = y * width + x;
		buffer[index * 2] = color >> 8;
		buffer[index * 2 + 1] = color;
	}

	void setPixel(int16_t x, int16_t y, uint8_t lo, uint8_t hi) {
		if (x >= width || y >= height || x < 0 || y < 0) return;
		uint16_t index = y * width + x;
		buffer[index * 2] = lo;
		buffer[index * 2 + 1] = hi;
	}

	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
		if (x >= width || y >= height) return;
		if(x < 0) {
			w += x;
			x = 0;
		}
		if(y < 0) {
			h += y;
			y = 0;
		}
		if (x + w > width) w = width - x;
		if (y + h > height) h = height - y;
		for(uint8_t x1 = x; x1 < x + w; x1++) {
			for(uint8_t y1 = y; y1 < y + h; y1++) {
				setPixel(x1, y1, color);
			}
		}
	}

	void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color, uint8_t thickness = 1) {
		int16_t steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep) {
			oreon_swap(x0, y0);
			oreon_swap(x1, y1);
		}

		if (x0 > x1) {
			oreon_swap(x0, x1);
			oreon_swap(y0, y1);
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

	void fillScreen(uint16_t color) {
		fillRect(0, 0, width, height, color);
	}

	void clear() {
		if(backgroundColor == textColor) {
			fillScreen(BLACK);
		} else {
			fillScreen(backgroundColor);
		}
	}

	void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t bitmap[], uint16_t color = textColor, uint16_t background = backgroundColor) {
		if (x >= width || y >= height || x < -w || y < -h) return;
		for (uint8_t x1 = 0; x1 < w; x1++) {
			for (uint8_t y1 = 0; y1 < h; y1++) {
				uint16_t index = x1 + y1 * w;
				if (bitRead(pgm_read_byte(&bitmap[index / 8]), 7 - index % 8)) { // TODO: optimize
					setPixel(x + (flip ? w - x1 - 1 : x1), y + y1, color);
				} else if(background != color) {
					setPixel(x + (flip ? w - x1 - 1 : x1), y + y1, background);
				}
			}
		}
	}

	void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[], int32_t alphaColor = -1) {
		if (x >= width || y >= height || x < -w || y < -h) return;
		for (uint8_t x1 = 0; x1 < w; x1++) {
			for (uint8_t y1 = 0; y1 < h; y1++) {
				uint16_t index = x1 + y1 * w;
				uint16_t color = (pgm_read_byte(&image[index * 2]) << 8) | pgm_read_byte(&image[index * 2 + 1]);
				if(color != alphaColor) {
					setPixel(x + (flip ? w - x1 - 1 : x1), y + y1, color);
				}
			}
		}
	}

#if defined(ESP8266)
#define BUFFPIXEL 20

	uint16_t read16(File &f) {
		uint16_t result;
		((uint8_t *)&result)[0] = f.read(); // LSB
		((uint8_t *)&result)[1] = f.read(); // MSB
		return result;
	}

	uint32_t read32(File &f) {
		uint32_t result;
		((uint8_t *)&result)[0] = f.read(); // LSB
		((uint8_t *)&result)[1] = f.read();
		((uint8_t *)&result)[2] = f.read();
		((uint8_t *)&result)[3] = f.read(); // MSB
		return result;
	}

	uint32_t drawImageFS(const char *filename, int16_t x, int16_t y, int32_t alphaColor = -1, int16_t imageY = 0, int16_t h = -1) {
		if (x >= width || y >= height) return 0;
		File file;
		uint8_t sdbuffer[3 * BUFFPIXEL];
		uint8_t buffidx = sizeof(sdbuffer);
		bool flip = true;

		if ((file = LittleFS.open(filename, "r")) == NULL) {
			Serial.println(String("File '") + filename + "' not found!");
			return 0;
		}

		if(read16(file) != 0x4D42) { // BMP signature
			Serial.println(String("Invalid bmp signature in file '") + filename + "'!");
			return 0;
		}
		read32(file); // Size
		read32(file); // Creator
		uint32_t bmpImageoffset = read32(file);
		read32(file); // Header size
		int w = read32(file), bmpHeight = read32(file);
		if(read16(file) != 1) {
			Serial.println("Invalid planes value! Should be 1!.");
			return 0;
		}
		if(read16(file) != 24 || read32(file) != 0) { // Seacond is uncompressed
			Serial.println("BMP format not recognized!");
			return 0;
		}
		uint32_t rowSize = (w * 3 + 3) & ~3;
		if(bmpHeight < 0) {
			bmpHeight = -bmpHeight;
			flip = false;
		}
		if(h < 0) h = bmpHeight;
	    for (int row = 0; row < h; row++) {
			uint32_t pos = bmpImageoffset + (flip ? (bmpHeight - row - imageY - 1) : row + imageY) * rowSize;
			if(file.position() != pos) {
				file.seek(pos);
				buffidx = sizeof(sdbuffer);
			}

			for (int col = 0; col < w; col++) {
				if (buffidx >= sizeof(sdbuffer)) {
					file.read(sdbuffer, sizeof(sdbuffer));
					buffidx = 0;
				}

				uint16_t color = rgb565(sdbuffer[buffidx++], sdbuffer[buffidx++], sdbuffer[buffidx++]);
				if(color != alphaColor) {
					setPixel(x + (oled::flip ? w - col - 1 : col), y + row, color);
				}
			}
		}
		file.close();
		return w << 16 | h;
	}
#endif

/*
	void drawImageEmbeded(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t image[], uint16_t offset = 0) {
		uint8_t colorCount = pgm_read_byte(&image[0]);
		uint8_t bitCount = pgm_read_byte(&image[1]);
		const uint8_t* colors = image + 2;
		const uint8_t* bmp = image + colorCount * 2 + 2;
		uint16_t bytes, bytesIndex = 65535;

		for (uint8_t x = 0; x < w; x++) {
			for (uint8_t y = 0; y < h; y++) {
				uint16_t index = (y + x * h + offset) * bitCount;
				if (index / 8 != bytesIndex) {
					if (index / 8 + 1 == bytesIndex) {
						bytes <<= 8;
						bytes |= pgm_read_byte(&bmp[index / 8 + 1]);
					} else {
						bytes = pgm_read_byte(&bmp[index / 8]);
						bytes <<= 8;
						bytes |= pgm_read_byte(&bmp[index / 8 + 1]);
					}
				}
				uint8_t colorIndex = (bytes << index % 8) >> (16 - bitCount);
				SPI.transfer(pgm_read_byte(&colors[colorIndex * 2]));
				SPI.transfer(pgm_read_byte(&colors[colorIndex * 2 + 1]));
			}
		}
	}*/

	void setFont(const uint8_t* font) {
		oled::font = font;
		fontW = pgm_read_byte(&font[0]);
		fontH = pgm_read_byte(&font[1]);
	}

	void setCursor(uint8_t x, uint8_t y) {
		cursorX = x;
		cursorY = y;
	}

	void setTextColor(uint16_t color, uint16_t background) {
		textColor = color;
		backgroundColor = background;
	}

	void setTextColor(uint16_t color) {
		textColor = backgroundColor = color;
	}

	uint8_t getStringWidth(String s) {
		return s.length() * fontW;
	}

	uint8_t getCharHeight() {
		return fontH;
	}

	void write(uint8_t c) {
		if(c > 31 && c < 127) {
      		drawBitmap(cursorX, cursorY, fontW, fontH, &font[(c - 32) * fontW * fontH / 8 + 2]);
		}
		cursorX += fontW;
		if(c == '\r') {
			cursorX = 0;
		}
		if(c == '\n' || cursorX > width - fontW) {
			cursorX = 0;
			cursorY += fontH;
		}
	}

	void print(String s) {
		for(uint16_t i = 0; i < s.length(); i++) {
			write(s[i]);
		}
	}

	void println(String s) {
		print(s);
		write('\n');
	}

	void update(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
		setAddressWindow(x, y, w, h);
		for (uint8_t y1 = y; y1 < y + h; y1++) {
			for (uint8_t x1 = x; x1 < x + w; x1++) {
				uint16_t index = x1 + y1 * width;
				uint16_t color = buffer[index * 2] << 8 | buffer[index * 2 + 1];

				uint16_t r = color >> 11;
				uint16_t g = color >> 5 & 0b00111111;
				uint16_t b = color & 0b00011111;
				r *= brightness;
				g *= brightness;
				b *= brightness;
				r /= 255;
				g /= 255;
				b /= 255;
				color = r | (g << 5) | (b << 11);

				writeSPI(color >> 8);
				writeSPI(color | 0xff);
			}
		}
		endWrite();
	}

	void update() {
		update(0, 0, width, height);
	}

	void begin() {
		pinMode(cs, OUTPUT);
		pinMode(dc, OUTPUT);
		pinMode(rst, OUTPUT);

		SPI.begin();
		SPI.setFrequency(30000000L);
		SPI.setBitOrder(MSBFIRST);
		digitalWrite(cs, LOW);

		digitalWrite(rst, HIGH);
		delay(500);
		digitalWrite(rst, LOW);
		delay(500);
		digitalWrite(rst, HIGH);
		delay(500);

		writeCmd(0xFD); //Set Command Lock
		writeData(0x12); //Unlock OLED driver IC MCU interface from entering command
		writeCmd(0xFD); //Set Command Lock
		writeData(0xB1); //Command A2,B1,B3,BB,BE,C1 accessible if in unlock state
		writeCmd(0xAE); //Sleep mode On (Display OFF)
		writeCmd(0xB3); //Front Clock Divider
		writeCmd(0xF1); // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
		writeCmd(0xCA); //Set MUX Ratio
		writeData(127);
		writeCmd(0xA0); //Set Re-map
		writeData(B01110100); //65k color
		//writeData(B10110100); //262k color
		//writeData(B11110100); //262k color, 16-bit format 2
		writeCmd(0x15); //Set Column
		writeData(0); //start
		writeData(width); //end
		writeCmd(0x75); //Set Row
		writeData(0); //start
		writeData(height); //end
		writeCmd(0xA1); //Set Display Start Line
		writeData(0);
		writeCmd(0xA2); //Set Display Offset
		writeData(0);
		writeCmd(0xB5); //Set GPIO
		writeData(0);
		writeCmd(0xAB); //Function Selection
		writeData(0x01); //Enable internal Vdd /8-bit parallel
		//writeData(B01000001); //Enable internal Vdd /Select 16-bit parallel interface
		writeCmd(0xB1); //Set Reset(Phase 1) /Pre-charge(Phase 2)
		//writeCmd(B00110010); //5 DCLKs / 3 DCLKs
		writeCmd(0x74);
		writeCmd(0xBE); //Set VCOMH Voltage
		writeCmd(0x05); //0.82 x VCC [reset]
		writeCmd(0xA6); //Reset to normal display
		writeCmd(0xC1); //Set Contrast
		writeData(0xC8); //Red contrast (reset=0x8A)
		writeData(0x80); //Green contrast (reset=0x51)
		writeData(0xC8); //Blue contrast (reset=0x8A)
		writeCmd(0xC7); //Master Contrast Current Control
		writeData(0x0F); //0-15
		writeCmd(0xB4); //Set Segment Low Voltage(VSL)
		writeData(0xA0);
		writeData(0xB5);
		writeData(0x55);
		writeCmd(0xB6); //Set Second Precharge Period
		writeData(0x01); //1 DCLKS
		writeCmd(0x9E); //Scroll Stop Moving
		writeCmd(0xAF); //Sleep mode On (Display ON)

		delay(100);
		brightness = 255;
		setCursor(0, 0);
		setFont(font8x12);
		setTextColor(WHITE);
		flip = false;
		clear();
		update();
	}
}