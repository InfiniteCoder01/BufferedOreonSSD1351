#pragma once
#include <OreonMath.hpp>
#include <OreonBSSD1351.hpp>
#include <LinkedList.h>
#include <initializer_list>
#include <typeinfo>

namespace TileEngine {
using namespace VectorMath;

#pragma region ECS
struct GameObject;
struct Component {
  virtual void setup(GameObject& object) {}
  virtual void update(GameObject& object) {}
  virtual void draw(GameObject& object) {}
  virtual void collide(GameObject& object, GameObject& other) {}
  virtual uint16_t getType() = 0;
};

struct GameObject {
  vec2f pos;
  vec2u size = 1;
  LinkedList<Component*> components;
  bool destroy = false;

  GameObject() = default;
  GameObject(vec2i pos) : pos(pos) {}
  ~GameObject() {
    while (components.size() > 0) {
      delete components[0];
      components.remove(0);
    }
  }

  template <class T> void addComponent(T* component) { components.add(component), component->setup(*this); }
  template <class T> inline void addComponent() { addComponent(new T()); }
  template <class T> T* getComponent(uint16_t type) {
    for (uint16_t i = 0; i < components.size(); i++) {
      if (components[i]->getType() == type) return (T*)components[i];
    }
    return nullptr;
  }

  template <class T> void removeComponent(T* component) {
    for (uint16_t i = 0; i < components.size(); i++) {
      if (components[i] == component) {
        delete components[i];
        components.remove(i);
        return;
      }
    }
  }

  void removeComponent(uint16_t type) {
    for (uint16_t i = 0; i < components.size(); i++) {
      if (components[i]->getType() == type) {
        delete components[i];
        components.remove(i);
        return;
      }
    }
  }

  void update() {
    for (uint16_t i = 0; i < components.size(); i++) components[i]->update(*this);
  }

  void draw() {
    for (uint16_t i = 0; i < components.size(); i++) components[i]->draw(*this);
  }

  void collide(GameObject& other) {
    for (uint16_t i = 0; i < components.size(); i++) components[i]->collide(*this, other);
  }
};

using LoadComponentFunction = Component* (*)(uint16_t type, const uint8_t*& data);
extern LoadComponentFunction loadUserComponent;
extern LinkedList<GameObject> objects;
#pragma endregion ECS

extern vec2i camera;

#pragma region Cache
struct Atlas {
  uint8_t width, height, frames;
  const uint8_t* atlas;

  void load(const uint8_t*& data) {
    width = pgm_read_byte(data++);
    height = pgm_read_byte(data++);
    frames = pgm_read_byte(data++);
    atlas = data;
    data += width * height * frames * 2;
  }

  vec2u size() const { return {width, height}; }
};

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
    objects = data;
    uint16_t nObjects = pgm_read_word(data);
    data += 2;
    for (uint32_t i = 0; i < nObjects; i++) {
      uint16_t objectSize = pgm_read_word(data);
      data += 2 + objectSize;
    }
  }

  vec2u size() { return {width, height}; }
};

extern Atlas* atlases;
extern Level* levels;
extern Level* level;
#pragma endregion Cache
#pragma region Interface
OREON_BSSD_DECL void drawTileFromAtlas(vec2i pos, uint8_t frame, const Atlas& atlas, bool flip = false) {
  oled::flip = flip;
  oled::drawImage(pos - camera, atlas.size(), atlas.atlas + atlas.width * atlas.height * 2 * frame, MAGENTA);
  oled::flip = false;
}

OREON_BSSD_DECL inline void drawTileFromAtlas(vec2i pos, uint8_t frame, uint8_t atlas, bool flip = false) { drawTileFromAtlas(pos, frame, atlases[atlas], flip); }
OREON_BSSD_DECL uint8_t getTile(int x, int y) { return pgm_read_byte(&level->data[x + y * level->width]); }
OREON_BSSD_DECL uint8_t getTile(vec2i pos) { return getTile(pos.x, pos.y); }

OREON_BSSD_DECL Component* loadComponent(uint16_t type, const uint8_t*& data);
OREON_BSSD_DECL GameObject& spawn(vec2i pos, std::initializer_list<Component*> components = {}) {
  objects.add(GameObject(pos));
  GameObject& object = objects[objects.size() - 1];
  for (auto component : components) object.addComponent(component);
  return object;
}

OREON_BSSD_DECL void loadLevel(uint8_t index) {
  objects.clear();
  level = &levels[index];
  const uint8_t* data = level->objects;
  uint16_t nObjects = pgm_read_word(data);
  data += 2;
  for (uint32_t i = 0; i < nObjects; i++) {
    data += 2;
    uint16_t x = pgm_read_word(data);
    data += 2;
    uint16_t y = pgm_read_word(data);
    data += 2;
    auto& object = spawn(vec2i(x, y));
    uint8_t nComponents = pgm_read_byte(data++);
    for (uint8_t i = 0; i < nComponents; i++) {
      uint16_t type = pgm_read_word(data);
      data += 2;
      object.addComponent(loadComponent(type, data));
    }
  }
}
#pragma endregion Interface
#pragma region Control
OREON_BSSD_DECL void load(const uint8_t* data, LoadComponentFunction loadUserComponent = nullptr) {
  /// Load atlases
  uint8_t nAtlases = pgm_read_byte(data++);
  delete[] atlases;
  atlases = new Atlas[nAtlases];
  for (int i = 0; i < nAtlases; i++) {
    atlases[i].load(data);
  }

  // Load level
  uint16_t nLevels = pgm_read_byte(data++);
  delete[] levels;
  levels = new Level[nLevels];
  for (int i = 0; i < nLevels; i++) levels[i].load(data);
  TileEngine::loadUserComponent = loadUserComponent;
}

OREON_BSSD_DECL void update() {
  for (uint16_t i = 0; i < objects.size(); i++) objects[i].update();
  for (int i = 0; i < objects.size() - 1; i++) {
    for (int j = i + 1; j < objects.size(); j++) {
      if (objects[i].destroy || objects[j].destroy) continue;
      if (Rect<int32_t>(objects[i].pos, objects[i].size).overlaps(Rect<int32_t>(objects[j].pos, objects[j].size))) {
        objects[i].collide(objects[j]);
        objects[j].collide(objects[i]);
      }
    }
  }
  for (int i = 0; i < objects.size(); i++) {
    if (objects[i].destroy) objects.remove(i--);
  }
}
#pragma endregion Control
#pragma region Draw
OREON_BSSD_DECL void draw() {
  const uint16_t levelAtlas = 0;
  for (int i = Math::max(camera.y / atlases[levelAtlas].height, 0); i < Math::min((camera.y + oled::height) / atlases[levelAtlas].height + 1, level->height); i++) {
    for (int j = Math::max(camera.x / atlases[levelAtlas].width, 0); j < Math::min((camera.x + oled::width) / atlases[levelAtlas].width + 1, level->width); j++) {
      if (getTile(j, i) != 0) drawTileFromAtlas(vec2i(j, i) * atlases[levelAtlas].size(), getTile(j, i) - 1, levelAtlas);
    }
  }

  for (int i = 0; i < objects.size(); i++) {
    objects[i].draw();
    // #ifdef DRAW_COLLIDERS
    //     oled::drawRect(objects[i].pos - camera, objects[i].size, RED);
    // #endif
  }
}
#pragma endregion Draw
#pragma region SaveAndLoad
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

OREON_BSSD_DECL uint16_t loadInt16(const uint8_t*& data) {
  uint16_t value = pgm_read_word(data);
  data += 2;
  return value;
}

OREON_BSSD_DECL uint16_t loadInt8(const uint8_t*& data) { return pgm_read_byte(data++); }
OREON_BSSD_DECL inline uint16_t loadAtlasIndex(const uint8_t*& data) { return loadInt8(data); }
OREON_BSSD_DECL inline uint16_t loadLevelIndex(const uint8_t*& data) { return loadInt8(data); }
#pragma endregion SaveAndLoad
#pragma region BuiltinComponents
enum class BuiltinComponents : uint16_t {
  AtlasRenderer,
  Count,
};

struct AtlasRenderer : public Component {
  uint8_t atlas;
  float frame = 0.f;
  bool flip = false;

  AtlasRenderer(uint16_t atlas) : Component(), atlas(atlas) {}
  void draw(GameObject& object) override {
    object.size = atlases[atlas].size();
    if (frame >= 0) drawTileFromAtlas(object.pos, frame, atlas, flip);
  }

  vec2u size() const { return atlases[atlas].size(); }
  uint16_t getType() override { return (uint16_t)BuiltinComponents::AtlasRenderer; }
};

OREON_BSSD_DECL Component* loadComponent(uint16_t type, const uint8_t*& data) {
  if (type == (uint16_t)BuiltinComponents::AtlasRenderer) return new AtlasRenderer(loadAtlasIndex(data)); // builtin/atlas
  return loadUserComponent(type, data);
}
#pragma endregion BuiltinComponents
} // namespace TileEngine
