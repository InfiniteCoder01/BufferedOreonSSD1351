#include <OreonBSSD1351.hpp>
#include <OreonBSSD1351Tile.hpp>

namespace oled {
/********** VARIABLES **********/
#if defined(SPI_HAS_TRANSACTION)
SPISettings _SPISettings;
#endif

uint8_t* buffer = new uint8_t[128 * 128 * 2ULL];
uint8_t* bufferW = buffer;

float textSize;
uint16_t textColor, backgroundColor;
uint8_t width = 128, height = 128, cursorX = 0, cursorY = 0, pageX = 0, pageR, fontW, fontH;
const uint8_t* font;
bool fontType;
int8_t cs, dc, rst;
int8_t textAlignment = LEFT;
bool flip, wrap;
} // namespace oled

namespace TileEngine {
vec2i camera;
Container::Vector<GameObject*> objects;
LoadComponentFunction loadUserComponent;

Atlas* atlases = nullptr;
Level* levels = nullptr;
Level* level = nullptr;
int32_t levelIndex = -1;
} // namespace TileEngine
