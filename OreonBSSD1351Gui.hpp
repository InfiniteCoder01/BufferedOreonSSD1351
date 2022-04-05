#pragma once
#include "OreonBSSD1351.hpp"

namespace gui {
	void centerText(String text, int y = -1) {
		oled::setCursor(oled::width / 2 - oled::getStringWidth(text) / 2, (y == -1 ? (oled::height / 2 - oled::getCharHeight() / 2) : y));
		oled::println(text);
	}

	void type(String text, uint16_t typeDelay = 100) {
		for(char c : text) {
			uint32_t t = millis();
			oled::print(String(c));
			oled::update();
			while(millis() - t < typeDelay) yield();
		}
	}

	void rightText(String text, int y) {
		oled::setCursor(oled::width - oled::getStringWidth(text), y);
		oled::println(text);
	}

	void drawList(String name, const String* elements, uint8_t elementCount, uint8_t pointer) {
		centerText(name, 0);
		for(int i = 0; i < elementCount; i++) {
			if(i == pointer) {
				oled::write('>');
			} else {
				oled::write(' ');
			}
			oled::println(elements[i]);
		}
	}

	void drawFPS() {
		static uint32_t t;
		static uint16_t prev;
		oled::setCursor(0, 0);
		uint16_t dt = millis() - t;
		t = millis();
		uint16_t approx = (prev + dt) / 2;
		prev = dt;
		oled::println("FPS: " + String(1000.0 / approx, 1));
	}
}