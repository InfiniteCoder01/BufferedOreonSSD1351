#pragma once
#include <stdint.h>
#include <math.h>

namespace Math {
template <typename T> inline bool inRange(T val, T _min, T _max) { return val >= _min && val < _max; }
template <typename T> inline bool inRangeW(T val, T _min, T size) { return inRange(val, _min, _min + size); }
template <typename T> inline int sign(T val) { return (val > 0) - (val < 0); }
inline int wrap(int val, int _max) {
  if (val < 0) return _max - abs(val) % _max;
  return val % _max;
}

template <typename T, typename T1> inline T align(T val, T1 alignment) { return int(val / alignment) * alignment; }
template <typename T, typename T1> inline T alignUp(T val, T1 alignment) { return int((val + alignment - 1) / alignment) * alignment; }

template <typename T, typename T1> inline T lerp(T a, T1 b, float t) { return a * (1 - t) + b * t; }
template <typename T, typename T1> inline void swap(T& a, T1& b) {
  T temp = a;
  a = b;
  b = temp;
}

template <typename T, typename T1> inline T min(T a, T1 b) { return a < b ? a : (T)b; }
template <typename T, typename T1> inline T max(T a, T1 b) { return a > b ? a : (T)b; }
template <typename T> inline T clamp(T val, T _min, T _max) { return min(max(val, _min), _max); }
template <typename T> inline T clampW(T val, T _min, T size) { return clamp(val, _min, _min + size); }
}  // namespace Math

namespace VectorMath {
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
  template<typename T1> bool operator>(const vec2<T1>& other) { return x > other.x && y > other.y; }
  template<typename T1> bool operator<(const vec2<T1>& other) { return x < other.x && y < other.y; }
  template<typename T1> bool operator>=(const vec2<T1>& other) { return x >= other.x && y >= other.y; }
  template<typename T1> bool operator<=(const vec2<T1>& other) { return x <= other.x && y <= other.y; }
  template<typename T1> bool operator==(const vec2<T1>& other) { return x == other.x && y == other.y; }
  template<typename T1> bool operator!=(const vec2<T1>& other) { return x != other.x || y != other.y; }

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
template <typename T> const vec3<T> vec3<T>::up = vec3<T>(0, -1, 0);
template <typename T> const vec3<T> vec3<T>::down = vec3<T>(0, 1, 0);
template <typename T> const vec3<T> vec3<T>::forward = vec3<T>(0, 0, 1);
template <typename T> const vec3<T> vec3<T>::backward = vec3<T>(0, 0, -1);

template <typename T> inline vec2<T> min(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(Math::min(a.x, b.x), Math::min(a.y, b.y)); }
template <typename T> inline vec2<T> max(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(Math::max(a.x, b.x), Math::max(a.y, b.y)); }
template <typename T> inline vec2<T> lerp(const vec2<T>& a, const vec2<T>& b, float t) { return vec2<T>(Math::lerp(a.x, b.x, t), Math::lerp(a.y, b.y, t)); }

template <typename T> inline vec3<T> min(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(Math::min(a.x, b.x), Math::min(a.y, b.y), Math::min(a.z, b.z)); }
template <typename T> inline vec3<T> max(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(Math::max(a.x, b.x), Math::max(a.y, b.y), Math::max(a.z, b.z)); }
template <typename T> inline vec3<T> lerp(const vec3<T>& a, const vec3<T>& b, float t) { return vec3<T>(Math::lerp(a.x, b.x, t), Math::lerp(a.y, b.y, t), Math::lerp(a.z, b.z, t)); }

// clang-format off
template<typename T> T inline sqrDistance(const vec2<T>& a, vec2<T>& b) { vec2<T> v = b - a; return v.sqrMagnitude(); }
template<typename T> T inline distance(const vec2<T>& a, vec2<T>& b) { vec2<T> v = b - a; return v.magnitude(); }

template<typename T> T inline sqrDistance(const vec3<T>& a, vec3<T>& b) { vec3<T> v = b - a; return v.sqrMagnitude(); }
template<typename T> T inline distance(const vec3<T>& a, vec3<T>& b) { vec3<T> v = b - a; return v.magnitude(); };
// clang-format on

template <typename T> inline T dot(const vec3<T>& a, const vec3<T>& b) { return a.dot(b); }
template <typename T> inline vec3<T> cross(const vec3<T>& a, const vec3<T>& b) { return a.cross(b); }

typedef vec2<int> vec2i;
typedef vec2<float> vec2f;
typedef vec3<int> vec3i;
typedef vec3<float> vec3f;
}  // namespace VectorMath
