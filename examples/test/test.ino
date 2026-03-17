#include <OreonBSSD1351.hpp>
// #include <OreonBSSD1351Gui.hpp>
#include "images.hpp"

OreonBSSD1351 oled;

String listElements[] = {
  "1st item",
  "2nd item",
  "3rd item",
  "Pointer will point here",
  "5th item"
};

void setup()  {
  Serial.begin(115200);
  SPI.setFrequency(SPI_SPEED); // OreonBSSD1351.hpp defines best SPI speed
  oled.begin(5, 17, 16); // cs, dc, rst

  oled.fillScreen(YELLOW); // after oled.begin() screen is black
  oled.update();
  delay(100);
  for (int i = 0; i < 10; i++) {
    const uint16_t w = random(128), h = random(128);
    oled.fillRect(random(128 - w), random(128 - h), w, h, random(0xffff)); // fillRect(x, y, width, height, color), you can also do drawRect(x, y, width, height, color)
    oled.update();
    delay(100);
  }

  delay(1000);
  oled.clear(); // same as fillScreen(textBackgroundColor), or if textBackgroundColor is transparent(same as textColor) fillScreen(BLACK)
  oled.drawBitmap(0, 0, 16, 16, bmp, RED, WHITE); // drawBitmap(x, y, width, height, bitmap, color = textColor, background = textBackgroundColor)
  oled.drawImage(16, 0, 16, 16, img); // drawImage(x, y, width, height, image, alphaColor = -1), alphaColor is transparency color. There is also fastDrawImage
  oled.setCursor(0, 16); // setCursor(x, y), at 0, 0 by default
  oled.setTextColor(WHITE); // setTextColor(textColor, backgroundColor = transparent(same as textColor)), by default (WHITE, transparent)
  oled.setFont(fontPico8); // setFont(font), included fonts are font5x8, font8x12 and fontPico8. font8x12 by default
  oled.textSize = 2; // textSize is float, 1 by default
  oled.println("Some text..."); // You can use plain print() without newline or write to write single char
  oled.textSize = 1;
  oled.update();

  delay(1000);
  oled.clear();
  oled.setCursor(0, 0);
  oled.flip = true;
  oled.println("GG!");
  oled.flip = false; // You can also disable wrap with "oled.wrap = false;", by default it's true
  oled.update();

  delay(1000);
  oled.clear();
  // gui::drawList("List name", listElements, 5, 3); // gui::drawList(listName, listElements, countOfElements, pointer)
  oled.update();

  delay(1000);
  oled.clear();
  oled.setCursor(0, 0);
  // gui::type("Typing text...", 300); // gui::type(text, typingDelay = 100ms by default)
  oled.update();
}

void loop() {
  oled.clear();
  // gui::centerText("Hi", 10); //  gui::centerText(text, yPos = center (-1));
  // gui::textAt("Hi!\nMultiline text here!", 20, 20); // gui::textAt(text, x, y) - Can draw left-aligned multiline text
  // gui::rightText("Right aligned text", 50); // gui::rightText(text, yPos); - align the text to right
  // gui::centerText("Hi\x7f\x80\x81\x82", 128 - oled.getCharHeight()); // Use getCharHeight() to get the height of the character in pixels, \x7f\x80 to create the x sign on pico8 font and  \x81\x82 to create the y sign on pico8 font
  // gui::textAt("Hi", 64 - oled.getStringWidth("Hi"), 64); // Use getStringWidth(text) to get the width of the string in pixels
  oled.drawLine(64, 0, 70, 10, RED, 3); // drawLine(x0, y0, x1, y1, color, thickness = 1)
  // gui::drawFPS();
  oled.update();
}
