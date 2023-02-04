#include <OreonBSSD1351.hpp>
#include <OreonBSSD1351Tile.hpp>

namespace oled {
/********** VARIABLES **********/
#if defined(SPI_HAS_TRANSACTION)
SPISettings _SPISettings = SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0);
#endif

uint8_t buffer[128 * 128 * 2UL];
uint8_t* bufferW = buffer;

float textSize;
uint16_t textColor, backgroundColor;
uint8_t width = 128, height = 128, cursorX = 0, cursorY = 0, pageX = 0, pageR, fontW, fontH;
const uint8_t* font;
bool fontType;
int8_t cs, dc, rst;
int8_t textAlignment = LEFT;
bool flip, wrap;
}  // namespace oled

namespace TileEngine {
uint16_t levelWidth, levelHeight;
vec2i camera;

const uint8_t* levelTiles;
uint16_t* objectToAtlas = nullptr;
LinkedList<Object> objects;
LoadObject loadObject;
UpdateObject updateObject;
CollideObject collideObject;

Atlas* atlases = nullptr;
Level* levels = nullptr;
bool ownAtlases = false;
}  // namespace TileEngine
