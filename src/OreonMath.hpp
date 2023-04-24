#pragma once
#include <math.h>
#include <stdint.h>

#ifdef min
#define OREON_MATH_MIN_DEF
#undef min
#endif

#ifdef max
#define OREON_MATH_MAX_DEF
#undef max
#endif

#ifdef abs
#define OREON_MATH_ABS_DEF
#undef abs
#endif

#ifdef round
#define OREON_MATH_ROUND_DEF
#undef round
#endif

#if __cplusplus < 201300
#define OREON_MATH_AUTO T1
#else
#define OREON_MATH_AUTO auto
#endif

namespace Math {
template <typename T1, typename T2, typename T3> inline bool inRange(T1 val, T2 _min, T3 _max) { return (val >= _min) && (val < _max); }
template <typename T1, typename T2, typename T3> inline bool inRangeW(T1 val, T2 _min, T3 size) { return inRange(val, _min, _min + size); }
template <typename T> inline int8_t sign(T val) { return (val > 0) - (val < 0); }
template <typename T> inline T abs(T val) { return val < 0 ? -val : val; }
inline int floor(float val) { return val < 0 ? (int)(val - 0.999999) : (int)val; }
inline int ceil(float val) { return val < 0 ? (int)val : (int)(val + 0.999999); }
inline int round(float val) { return static_cast<int>(lround(val)); }
template <typename T1, typename T2> inline OREON_MATH_AUTO wrap(T1 val, T2 _max) {
  if (val < 0) return _max - abs(val) % _max;
  return val % _max;
}

template <typename T1, typename T2> inline T2 align(T1 val, T2 alignment) { return int(val / alignment) * alignment; }
template <typename T1, typename T2> inline T2 alignUp(T1 val, T2 alignment) { return int((val + alignment - 1) / alignment) * alignment; }

template <typename T1, typename T2> inline float lerp(T1 a, T2 b, float t) { return a * (1 - t) + b * t; }
template <typename T1, typename T2> inline float lerp255(T1 a, T2 b, uint8_t t) { return (a * (255 - t) + b * t) / 255; }
template <typename T1, typename T2> inline void swap(T1& a, T2& b) {
  T1 temp = a;
  a = b;
  b = temp;
}

template <typename T1, typename T2> inline OREON_MATH_AUTO min(T1 a, T2 b) { return a < b ? a : b; }
template <typename T1, typename T2> inline OREON_MATH_AUTO max(T1 a, T2 b) { return a > b ? a : b; }
template <typename T1, typename T2, typename T3> inline OREON_MATH_AUTO clamp(T1 val, T2 _min, T3 _max) { return min(max(val, _min), _max); }
template <typename T1, typename T2, typename T3> inline OREON_MATH_AUTO clampW(T1 val, T2 _min, T3 size) { return clamp(val, _min, _min + size); }
static float smoothstep(float _min, float _max, float t) {
  t = clamp((t - _min) / (_max - _min), 0, 1);
  return t * t * (3 - 2 * t);
}
} // namespace Math

namespace VectorMath {
// ---------- Vec2 ----------
template <typename T> struct vec2 {
  static const vec2<T> zero;
  static const vec2<T> one;
  static const vec2<T> left;
  static const vec2<T> right;
  static const vec2<T> up;
  static const vec2<T> down;

  T x, y;

  vec2() : vec2(0) {}
  vec2(T n) : vec2(n, n) {}
  vec2(T x, T y) : x(x), y(y) {}
  vec2(const vec2<T>& other) : vec2(other.x, other.y) {}

  // clang-format off
  vec2<T> operator-() const { return vec2<T>(-x, -y); }
  vec2<T> operator+(T value) const { return vec2<T>(x + value, y + value); }
  vec2<T> operator-(T value) const { return vec2<T>(x - value, y - value); }
  vec2<T> operator*(float value) const { return vec2<T>(x * value, y * value); }
  vec2<T> operator/(float value) const { return vec2<T>(x / value, y / value); }
  vec2<T>& operator+=(T value) { x += value; y += value; return *this; }
  vec2<T>& operator-=(T value) { x -= value; y -= value; return *this; }
  vec2<T>& operator*=(float value) { x *= value; y *= value; return *this; }
  vec2<T>& operator/=(float value) { x /= value; y /= value; return *this; }
  vec2<T> operator+(const vec2<T>& other) const { return vec2<T>(x + other.x, y + other.y); }
  vec2<T> operator-(const vec2<T>& other) const { return vec2<T>(x - other.x, y - other.y); }
  vec2<T> operator*(const vec2<T>& other) const { return vec2<T>(x * other.x, y * other.y); }
  vec2<T> operator/(const vec2<T>& other) const { return vec2<T>(x / other.x, y / other.y); }
  vec2<T>& operator+=(const vec2<T>& other) { x += other.x; y += other.y; return *this; }
  vec2<T>& operator-=(const vec2<T>& other) { x -= other.x; y -= other.y; return *this; }
  vec2<T>& operator*=(const vec2<T>& other) { x = x * other.x; y = y * other.y; return *this; }
  vec2<T>& operator/=(const vec2<T>& other) { x = x / other.x; y = y / other.y; return *this; }
  vec2<T>& operator=(const vec2<T>& other) { x = other.x; y = other.y; return *this; }
  template<typename T1> bool operator>(const vec2<T1>& other) const { return x > other.x && y > other.y; }
  template<typename T1> bool operator<(const vec2<T1>& other) const { return x < other.x && y < other.y; }
  template<typename T1> bool operator>=(const vec2<T1>& other) const { return x >= other.x && y >= other.y; }
  template<typename T1> bool operator<=(const vec2<T1>& other) const { return x <= other.x && y <= other.y; }
  template<typename T1> bool operator==(const vec2<T1>& other) const { return x == other.x && y == other.y; }
  template<typename T1> bool operator!=(const vec2<T1>& other) const { return x != other.x || y != other.y; }

  bool operator>(T value) const { return x > value && y > value; }
  bool operator<(T value) const { return x < value && y < value; }
  bool operator>=(T value) const { return x >= value && y >= value; }
  bool operator<=(T value) const { return x <= value && y <= value; }
  bool operator==(T value) const { return x == value && y == value; }
  bool operator!=(T value) const { return x != value || y != value; }

  T sqrMagnitude() const { return x * x + y * y; }
  T magnitude() const { return sqrt(sqrMagnitude()); }
  vec2<T> normalized() const { return operator/(magnitude()); }
  vec2<T>& normalize() { return operator/=(magnitude()); }
  T dot(const vec2<T>& other) const { return x * other.x + y * other.y; }
  T cross(const vec2<T>& other) const { return x * other.y - y * other.x; }
  void rotate(float angle) { x = x * cos(angle) - y * sin(angle); y = x * sin(angle) + y * cos(angle); }
  // clang-format on

  template <typename T1> operator vec2<T1>() const { return vec2<T1>(x, y); }
};

// ---------- Vec3 ----------
template <typename T> struct vec3 {
  static const vec3<T> zero;
  static const vec3<T> one;
  static const vec3<T> left;
  static const vec3<T> right;
  static const vec3<T> up;
  static const vec3<T> down;
  static const vec3<T> forward;
  static const vec3<T> backward;

  T x, y, z;

  vec3() : vec3(0) {}
  vec3(T n) : vec3(n, n, n) {}
  vec3(T x, T y, T z) : x(x), y(y), z(z) {}
  vec3(const vec3<T>& other) : vec3(other.x, other.y, other.z) {}

  // clang-format off
  vec3<T> operator-() const { return vec3<T>(-x, -y, -z); }
  vec3<T> operator+(float value) const { return vec3<T>(x + value, y + value, z + value); }
  vec3<T> operator-(float value) const { return vec3<T>(x - value, y - value, z - value); }
  vec3<T> operator*(float value) const { return vec3<T>(x * value, y * value, z * value); }
  vec3<T> operator/(float value) const { return vec3<T>(x / value, y / value, z / value); }
  vec3<T>& operator+=(float value) { x += value; y += value; z += value; return *this; }
  vec3<T>& operator-=(float value) { x -= value; y -= value; z -= value; return *this; }
  vec3<T>& operator*=(float value) { x *= value; y *= value; z *= value; return *this; }
  vec3<T>& operator/=(float value) { x /= value; y /= value; z /= value; return *this; }
  vec3<T> operator+(const vec3<T>& other) const { return vec3<T>(x + other.x, y + other.y, z + other.z); }
  vec3<T> operator-(const vec3<T>& other) const { return vec3<T>(x - other.x, y - other.y, z - other.z); }
  vec3<T> operator*(const vec3<T>& other) const { return vec3<T>(x * other.x, y * other.y, z * other.z); }
  vec3<T> operator/(const vec3<T>& other) const { return vec3<T>(x / other.x, y / other.y, z / other.z); }
  vec3<T>& operator+=(const vec3<T>& other) { x += other.x; y += other.y; z += other.z; return *this; }
  vec3<T>& operator-=(const vec3<T>& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
  vec3<T>& operator*=(const vec3<T>& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
  vec3<T>& operator/=(const vec3<T>& other) { x /= other.x; y /= other.y; z /= other.z; return *this; }
  vec3<T>& operator=(const vec3<T>& other) { x = other.x; y = other.y; z = other.z; return *this; }
  template<typename T1> bool operator==(const vec3<T1>& other) const { return x == other.x && y == other.y && z == other.z; }
  template<typename T1> bool operator!=(const vec3<T1>& other) const { return x != other.x || y != other.y || z != other.z; }
  template<typename T1> bool operator>=(const vec3<T1>& other) const { return x >= other.x && y >= other.y && z >= other.z; }
  template<typename T1> bool operator<=(const vec3<T1>& other) const { return x <= other.x && y <= other.y && z <= other.z; }
  template<typename T1> bool operator>(const vec3<T1>& other) const { return x > other.x && y > other.y && z > other.z; }
  template<typename T1> bool operator<(const vec3<T1>& other) const { return x < other.x && y < other.y && z < other.z; }
  bool operator==(T value) const { return x == value && y == value && z == value; }
  bool operator!=(T value) const { return x != value || y != value || z != value; }
  bool operator<=(T value) const { return x <= value && y <= value && z <= value; }
  bool operator>=(T value) const { return x >= value && y >= value && z >= value; }
  bool operator<(T value) const { return x < value && y < value && z < value; }
  bool operator>(T value) const { return x > value && y > value && z > value; }

  T sqrMagnitude() const { return x * x + y * y + z * z; }
  float magnitude() const { return sqrt((float)sqrMagnitude()); }
  vec3<T> normalized() const { return operator/(magnitude()); }
  vec3<T>& normalize() { return operator/=(magnitude()); }
  T dot(const vec3<T>& other) const { return x * other.x + y * other.y + z * other.z; }
  vec3<T> cross(const vec3<T>& other) const { return vec3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); }
  // clang-format on

  template <typename T1> operator vec3<T1>() const { return vec3<T1>(x, y, z); }
};

// ---------- Vector Math ----------
template <typename T> const vec2<T> vec2<T>::zero = vec2<T>(0);
template <typename T> const vec2<T> vec2<T>::one = vec2<T>(1);
template <typename T> const vec2<T> vec2<T>::left = vec2<T>(-1, 0);
template <typename T> const vec2<T> vec2<T>::right = vec2<T>(1, 0);
template <typename T> const vec2<T> vec2<T>::up = vec2<T>(0, -1);
template <typename T> const vec2<T> vec2<T>::down = vec2<T>(0, 1);
template <typename T> const vec3<T> vec3<T>::zero = vec3<T>(0);
template <typename T> const vec3<T> vec3<T>::one = vec3<T>(1);
template <typename T> const vec3<T> vec3<T>::left = vec3<T>(-1, 0, 0);
template <typename T> const vec3<T> vec3<T>::right = vec3<T>(1, 0, 0);
template <typename T> const vec3<T> vec3<T>::up = vec3<T>(0, 1, 0);
template <typename T> const vec3<T> vec3<T>::down = vec3<T>(0, -1, 0);
template <typename T> const vec3<T> vec3<T>::forward = vec3<T>(0, 0, 1);
template <typename T> const vec3<T> vec3<T>::backward = vec3<T>(0, 0, -1);

template <typename T> inline vec2<T> min(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(Math::min(a.x, b.x), Math::min(a.y, b.y)); }
template <typename T> inline vec2<T> max(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(Math::max(a.x, b.x), Math::max(a.y, b.y)); }
template <typename T> inline vec3<T> min(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(Math::min(a.x, b.x), Math::min(a.y, b.y), Math::min(a.z, b.z)); }
template <typename T> inline vec3<T> max(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(Math::max(a.x, b.x), Math::max(a.y, b.y), Math::max(a.z, b.z)); }

template <typename T> inline vec2<T> lerp(const vec2<T>& a, const vec2<T>& b, float t) { return vec2<T>(Math::lerp(a.x, b.x, t), Math::lerp(a.y, b.y, t)); }
template <typename T> inline vec3<T> lerp(const vec3<T>& a, const vec3<T>& b, float t) { return vec3<T>(Math::lerp(a.x, b.x, t), Math::lerp(a.y, b.y, t), Math::lerp(a.z, b.z, t)); }

// clang-format off
template<typename T> T inline sqrDistance(const vec2<T>& a, const vec2<T>& b) { vec2<T> v = b - a; return v.sqrMagnitude(); }
template<typename T> T inline distance(const vec2<T>& a, const vec2<T>& b) { vec2<T> v = b - a; return v.magnitude(); }

template<typename T> T inline sqrDistance(const vec3<T>& a, const vec3<T>& b) { vec3<T> v = b - a; return v.sqrMagnitude(); }
template<typename T> T inline distance(const vec3<T>& a, const vec3<T>& b) { vec3<T> v = b - a; return v.magnitude(); };
// clang-format on

template <typename T> inline T dot(const vec3<T>& a, const vec3<T>& b) { return a.dot(b); }
template <typename T> inline vec3<T> cross(const vec3<T>& a, const vec3<T>& b) { return a.cross(b); }

template <typename T> inline vec2<T> abs(vec2<T> v) { return vec2<T>(Math::abs(v.x), Math::abs(v.y)); }
template <typename T> inline vec3<T> abs(vec3<T> v) { return vec3<T>(Math::abs(v.x), Math::abs(v.y), Math::abs(v.z)); }

template <typename T> inline vec2<int> sign(vec2<T> v) { return vec2<int>(Math::sign(v.x), Math::sign(v.y)); }
template <typename T> inline vec3<int> sign(vec3<T> v) { return vec3<int>(Math::sign(v.x), Math::sign(v.y), Math::sign(v.z));  }

inline vec2<int> floor(vec2<float> v) { return vec2<int>(Math::floor(v.x), Math::floor(v.y)); }
inline vec3<int> floor(vec3<float> v) { return vec3<int>(Math::floor(v.x), Math::floor(v.y), Math::floor(v.z)); }

inline vec2<int> ceil(vec2<float> v) { return vec2<int>(Math::ceil(v.x), Math::ceil(v.y)); }
inline vec3<int> ceil(vec3<float> v) { return vec3<int>(Math::ceil(v.x), Math::ceil(v.y), Math::ceil(v.z)); }

inline vec2<int> round(vec2<float> v) { return vec2<int>(Math::round(v.x), Math::round(v.y)); }
inline vec3<int> round(vec3<float> v) { return vec3<int>(Math::round(v.x), Math::round(v.y), Math::round(v.z)); }

template <typename T> static vec3<T> rotateAround(vec3<T> v, vec3<T> axis, float angle) {
  vec3<T> parallel = axis * dot(v, axis) / dot(axis, axis);
  vec3<T> perpendicular = v - parallel;
  vec3<T> w = cross(axis, perpendicular);
  return (parallel + perpendicular * cos(angle) + w.normalized() * perpendicular.magnitude() * sin(angle));
}

// ---------- Vector Types ----------
typedef vec2<int> vec2i;
typedef vec2<unsigned int> vec2u;
typedef vec2<float> vec2f;
typedef vec3<int> vec3i;
typedef vec3<unsigned int> vec3u;
typedef vec3<float> vec3f;

// ---------- Rect ----------
template <typename T> struct Rect {
  static const Rect<T> zero;

  T x, y, width, height;

  Rect() : Rect(0, 0, 0, 0) {}
  Rect(T x, T y, T width, T heigt) : x(x), y(y), width(width), height(heigt) {
    if (width < 0) x += width, width *= -1;
    if (heigt < 0) y += heigt, heigt *= -1;
  }
  Rect(const vec2<T>& position, const vec2<T>& size) : Rect(position.x, position.y, size.x, size.y) {}
  Rect(const Rect<T>& other) : Rect(other.x, other.y, other.width, other.height) {}

  static Rect<T> centered(vec2<T> center, vec2<T> size) { return Rect<T>(center - size / 2, size); }

  T centerX() const { return x + width / 2; }
  T centerY() const { return y + height / 2; }

  vec2<T> position() const { return vec2<T>(x, y); }
  vec2<T> size() const { return vec2<T>(width, height); }
  vec2<T> center() const { return position() + size() / 2; }
  T area() const { return width * height; }
  bool empty() const { return area() == 0; }

  T left() const { return x; }
  T right() const { return x + width; }
  T top() const { return y; }
  T bottom() const { return y + height; }

  vec2<T> tl() const { return vec2<T>(x, y); }
  vec2<T> tr() const { return vec2<T>(x + width, y); }
  vec2<T> bl() const { return vec2<T>(x, y + height); }
  vec2<T> br() const { return vec2<T>(x + width, y + height); }

  bool contains(const vec2<T>& v) const { return v >= tl() && v < br(); }
  bool contains(const Rect<T>& other) const { return other.tl() >= tl() && other.br() <= br(); }
  bool overlaps(const Rect<T>& other) const { return left() < other.right() && right() > other.left() && top() < other.bottom() && bottom() > other.top(); }
  bool intersects(const Rect<T>& other) const { return overlaps(other); }
  Rect<T> intersection(const Rect<T> other) const {
    if (!intersects(other)) return zero;
    vec2<T> tl = max(tl(), other.tl()), br = min(br(), other.br());
    return Rect<T>(tl, br - tl);
  }

  Rect<T> common(const Rect<T> other) const {
    vec2<T> tl = min(tl(), other.tl()), br = max(br(), other.br());
    return Rect<T>(tl, br - tl);
  }

  Rect<T> common(const vec2<T> v) const {
    vec2<T> tl = min(tl(), v), br = max(br(), v);
    return Rect<T>(tl, br - tl);
  }

  Rect<T> scaleCenter(float scale) { return Rect<T>(center() - size() * scale / 2, size() * scale); }
  Rect<T> border(uint32_t border) { return Rect<T>(position() - border, size() + border * 2); }

  // clang-format off
  Rect<T> operator+(T v) const { return Rect<T>(x + v, y + v, width, height); }
  Rect<T> operator-(T v) const { return Rect<T>(x - v, y - v, width, height); }
  Rect<float> operator*(float v) const { return Rect<T>(x, y, width * v, height * v); }
  Rect<float> operator/(float v) const { return Rect<T>(x, y, width / v, height / v); }
  Rect<T> operator+(const vec2<T>& v) const { return Rect<T>(x + v.x, y + v.y, width, height); }
  Rect<T> operator-(const vec2<T>& v) const { return Rect<T>(x - v.x, y - v.y, width, height); }
  Rect<float> operator*(const vec2<float>& v) const { return Rect<T>(x, y, width * v.x, height * v.y); }
  Rect<float> operator/(const vec2<float>& v) const { return Rect<T>(x, y, width / v.x, height / v.y); }
  Rect<T>& operator+=(T v) { x += v; y += v; return *this; }
  Rect<T>& operator-=(T v) { x -= v; y -= v; return *this; }
  Rect<float>& operator*=(float v) { width *= v; height *= v; return *this; }
  Rect<float>& operator/=(float v) { width /= v; height /= v; return *this; }
  Rect<T>& operator+=(const vec2<T>& v) { x += v.x; y += v.y; return *this; }
  Rect<T>& operator-=(const vec2<T>& v) { x -= v.x; y -= v.y; return *this; }
  Rect<float>& operator*=(const vec2<float>& v) { width *= v.x; height *= v.y; return *this; }
  Rect<float>& operator/=(const vec2<float>& v) { width /= v.x; height /= v.y; return *this; }
  template<typename T1> bool operator==(const Rect<T>& other) const { return x == other.x && y == other.y && width == other.width && height == other.height; }
  template<typename T1> bool operator!=(const Rect<T>& other) const { return x != other.x || y != other.y || width != other.width || height != other.height; }

  bool operator&&(const Rect<T>& other) const { return overlaps(other); }
  bool operator&&(const vec2<T>& v) const { return contains(v); }

  Rect<T> operator&(const Rect<T>& other) const { return intersection(other); }
  Rect<T> operator|(const Rect<T>& other) const { return common(other); }
  Rect<T> operator|(const vec2<T>& v) const { return common(v); }
  Rect<T>& operator&=(const Rect<T>& other) { return *this = intersection(other); }
  Rect<T>& operator|=(const Rect<T>& other) { return *this = common(other); }
  Rect<T>& operator|=(const vec2<T>& v) { return *this = common(v); }
  // clang-format on

  template <typename T1> operator Rect<T1>() const { return Rect<T1>(x, y, width, height); }
};

template <typename T> const Rect<T> Rect<T>::zero = Rect<T>(0, 0, 0, 0);
} // namespace VectorMath

#ifdef OREON_MATH_MIN_DEF
#define min Math::min
#endif

#ifdef OREON_MATH_MAX_DEF
#define max Math::max
#endif

#ifdef OREON_MATH_ABS_DEF
#define abs Math::abs
#endif

#ifdef OREON_MATH_ROUND_DEF
#define round Math::round
#endif
