#pragma once
#include <OreonMath.hpp>
#include <OreonBSSD1351.hpp>
#include <LinkedList.h>

namespace TileEngine {
/*          DATA          */
using namespace VectorMath;
struct Object {
  vec2f pos;
  vec2i size = vec2i::one;
  uint16_t type;
  float frame = 0;
  bool destroy = false;
  bool flip;
  void* data;

  Object() = default;
  Object(vec2i pos, uint16_t type, void* data) : pos(pos), type(type), data(data) {}
  ~Object() {
    // if (data) delete (char*)data; TODO: MemoryLeak
  }
};

using LoadObject = void* (*)(uint16_t type, const uint8_t*& data);
using UpdateObject = void (*)(Object& object);
using CollideObject = void (*)(Object& a, Object& b);
extern uint16_t levelWidth, levelHeight;
extern vec2i camera;

extern const uint8_t* levelTiles;
extern uint16_t* objectToAtlas;
extern LinkedList<Object> objects;
extern LoadObject loadObject;
extern UpdateObject updateObject;
extern CollideObject collideObject;

inline VectorMath::vec2i levelSize() { return VectorMath::vec2i(levelWidth, levelHeight); }

/*          CACHE          */
struct Atlas {
  uint8_t width, height, frames;
  const uint8_t* atlas;
  struct Tileset {
    uint8_t nPatches;
    uint8_t* patches;
    uint8_t* colliders;
  }* tileset;

  void load(const uint8_t*& data) {
    frames = pgm_read_byte(data++);
    width = pgm_read_byte(data++);
    height = pgm_read_byte(data++);
    atlas = data;
    data += width * height * frames * 2;
    if (pgm_read_byte(data++)) {
      tileset = new Tileset();
      tileset->nPatches = pgm_read_byte(data++);
      tileset->patches = new uint8_t[tileset->nPatches];
      for (int i = 0; i < tileset->nPatches; i++) tileset->patches[i] = pgm_read_byte(data++);
      if (pgm_read_byte(data++)) {
        uint16_t colliderMapSize = frames / 8 + (frames % 8 != 0);
        tileset->colliders = new uint8_t[colliderMapSize];
        memcpy_P(tileset->colliders, data, colliderMapSize);
        data += colliderMapSize;
      } else tileset->colliders = nullptr;
    } else tileset = nullptr;
  }

  void skip(const uint8_t*& data) {
    data += 3 + width * height * frames * 2 + 1;
    if (tileset) {
      data += 1 + tileset->nPatches + 1;
      if (tileset->colliders) data += frames / 8 + (frames % 8 != 0);
    }
  }

  ~Atlas() {
    if (tileset) {
      delete[] tileset->patches;
      if (tileset->colliders) delete[] tileset->colliders;
      delete tileset;
    }
  }

  ALWAYS_INLINE vec2i size() const { return vec2i(width, height); }
  bool collide(uint8_t frame) { return tileset->colliders[frame / 8] & (1 << (frame % 8)); }
};

OREON_BSSD_DECL void spawn(vec2i pos, uint8_t type, void* data = nullptr);
struct Level {
  uint16_t width, height;
  const uint8_t* data;
  const uint8_t* objects;

  void load(const uint8_t*& data) {
    width = pgm_read_word(data);
    data += 2;
    height = pgm_read_word(data);
    data += 2;
    this->data = data;
    data += width * height;
    uint32_t objectsDataSize = pgm_read_dword(data);
    data += 4;
    objects = data;
    data += objectsDataSize;
  }

  void activate(bool loadObjects = true) {
    levelWidth = width;
    levelHeight = height;
    levelTiles = data;
    TileEngine::objects.clear();
    if (loadObjects) {
      const uint8_t* ptr = objects;
      uint16_t nObjects = pgm_read_word(ptr);
      ptr += 2;
      for (int i = 0; i < nObjects; i++) {
        uint16_t x = pgm_read_word(ptr), y = pgm_read_word(ptr + 2), type = pgm_read_word(ptr + 4);
        ptr += 6;
        spawn(vec2i(x, y), type, loadObject(type, ptr));
      }
    }
  }
};
extern Atlas* atlases;
extern Level* levels;
extern bool ownAtlases;

OREON_BSSD_DECL void drawTileFromAtlas(vec2i pos, uint8_t frame, const Atlas& atlas, bool flip = false) {
  oled::flip = flip;
  oled::drawImage(pos - camera, atlas.size(), atlas.atlas + atlas.width * atlas.height * 2 * frame, MAGENTA);
  oled::flip = false;
}

/*          INTERFACE          */
OREON_BSSD_DECL uint8_t getTile(int x, int y) { return pgm_read_byte(&levelTiles[x + y * levelWidth]); }
OREON_BSSD_DECL uint8_t getTile(vec2i pos) { return getTile(pos.x, pos.y); }
OREON_BSSD_DECL void spawn(vec2i pos, uint8_t type, void* data) { objects.add(Object(pos, type, data)); }
OREON_BSSD_DECL void loadLevel(uint16_t level, bool loadObjects = true) { levels[level].activate(loadObjects); }

/*          CONTROL          */
OREON_BSSD_DECL const uint8_t* loadAtlases(const uint8_t* atlasData, Atlas*& output) {
  uint16_t nAtlases = pgm_read_word(atlasData);
  atlasData += 2;
  output = new Atlas[nAtlases];
  for (int i = 0; i < nAtlases; i++) {
    output[i].load(atlasData);
  }
  return atlasData;
}

OREON_BSSD_DECL void begin(const uint8_t* atlasData, const uint8_t* levelsPtr, LoadObject loadObject, UpdateObject updateObject, CollideObject collideObject = nullptr, Atlas* externalAtlases = nullptr) {
  {  // Load atlases
    if (externalAtlases) {
      TileEngine::atlases = externalAtlases;
      uint16_t nAtlases = pgm_read_word(atlasData);
      atlasData += 2;
      for (int i = 0; i < nAtlases; i++) TileEngine::atlases[i].skip(atlasData);
      ownAtlases = false;
    } else {
      if (TileEngine::atlases && ownAtlases) delete[] TileEngine::atlases;
      atlasData = loadAtlases(atlasData, TileEngine::atlases);
      ownAtlases = true;
    }

    uint16_t nObjects = pgm_read_word(atlasData);
    atlasData += 2;
    if (objectToAtlas) delete[] objectToAtlas;
    objectToAtlas = new uint16_t[nObjects];
    for (uint16_t i = 0; i < nObjects; i++) {
      objectToAtlas[i] = pgm_read_word(atlasData);
      atlasData += 2;
    }
  }

  {  // Load level
    uint16_t nLevels = pgm_read_word(levelsPtr);
    levelsPtr += 2;
    if (levels) delete[] levels;
    levels = new Level[nLevels];
    for (int i = 0; i < nLevels; i++) levels[i].load(levelsPtr);
    TileEngine::loadObject = loadObject;
    TileEngine::updateObject = updateObject;
    TileEngine::collideObject = collideObject;
  }
}

OREON_BSSD_DECL void update() {
  for (int i = 0; i < objects.size(); i++) {
    objects[i].size = atlases[objectToAtlas[objects[i].type]].size();
    updateObject(objects[i]);
  }
  if (collideObject) {
    for (int i = 0; i < objects.size() - 1; i++) {
      for (int j = i + 1; j < objects.size(); j++) {
        if (objects[i].destroy || objects[j].destroy) continue;
        if (objects[i].pos.x < objects[j].pos.x + objects[j].size.x && objects[i].pos.x + objects[i].size.x > objects[j].pos.x && objects[i].pos.y < objects[j].pos.y + objects[j].size.y && objects[i].pos.y + objects[i].size.y > objects[j].pos.y) {
          collideObject(objects[i], objects[j]);
          collideObject(objects[j], objects[i]);
        }
      }
    }
  }
  for (int i = 0; i < objects.size(); i++) {
    if (objects[i].destroy) objects.remove(i--);
  }
}

static void drawQuater(const Atlas& atlas, vec2i pos, uint8_t tile, vec2i quater) {
  vec2i delta = quater * 2 - 1;

  if (getTile(pos + vec2i(delta.x, 0)) != tile) tile += getTile(pos + vec2i(0, delta.y)) != tile ? 1 : 3;
  else if (getTile(pos + vec2i(0, delta.y)) != tile) tile += 2;
  uint16_t halfTileSize = atlas.width * atlas.height;
  uint32_t ptr = halfTileSize * 2 * (tile - 1);
  oled::drawImage((pos * 2 + quater) * atlas.size() / 2 - camera, atlas.size() / 2, atlas.atlas + ptr + quater.x * atlas.width + quater.y * halfTileSize, MAGENTA, atlas.width / 2);
}

static void drawPatch(const Atlas& atlas, vec2i pos, uint8_t tile) {
  drawQuater(atlas, pos, tile, vec2i(0, 0));
  drawQuater(atlas, pos, tile, vec2i(1, 0));
  drawQuater(atlas, pos, tile, vec2i(0, 1));
  drawQuater(atlas, pos, tile, vec2i(1, 1));
}

OREON_BSSD_DECL void draw() {
  uint16_t levelAtlas = 0;
  for (int i = 0; i < levelHeight; i++) {
    for (int j = 0; j < levelWidth; j++) {
      if (getTile(j, i) != 0) {
        bool found = false;
        for (int k = 0; k < atlases[levelAtlas].tileset->nPatches; k++) {
          if (getTile(j, i) == atlases[levelAtlas].tileset->patches[k]) {
            drawPatch(atlases[levelAtlas], vec2i(j, i), getTile(j, i));
            found = true;
            break;
          }
        }
        if (!found) drawTileFromAtlas(vec2i(j, i) * atlases[levelAtlas].size(), getTile(j, i) - 1, atlases[levelAtlas]);
      }
    }
  }

  for (int i = 0; i < objects.size(); i++) {
    if (objects[i].frame >= 0) drawTileFromAtlas(objects[i].pos, objects[i].frame, atlases[objectToAtlas[objects[i].type]], objects[i].flip);
#ifdef DRAW_COLLIDERS
    oled::drawRect(objects[i].pos - camera, objects[i].size, RED);
#endif
  }
}

OREON_BSSD_DECL String loadString(const uint8_t*& data) {
  String str;
  while (true) {
    char c = pgm_read_byte(data++);
    if (c == '\0') break;
    str += c;
  }
  return str;
}

OREON_BSSD_DECL uint32_t loadInt(const uint8_t*& data) {
  uint32_t value = pgm_read_dword(data);
  data += 4;
  return value;
}
}  // namespace TileEngine
