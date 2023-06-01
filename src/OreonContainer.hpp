#pragma once
#include <initializer_list>
#include <memory.h>
#include <assert.h>
#include <stddef.h>

#define OreonContainer_max(a, b) ((a) > (b) ? (a) : (b))
#define OreonContainer_min(a, b) ((a) < (b) ? (a) : (b))

namespace Container {
template <typename T> class Vector {
public:
  // * Constructors & Assignment
  Vector() : Vector(0) {}
  Vector(size_t size, const T& value = T()) : m_Size(size), m_Capacity(m_Size) {
    m_Data = new T[m_Capacity];
    for (size_t i = 0; i < size; i++) m_Data[i] = value;
  }

  Vector(Vector&& other) noexcept : m_Capacity(other.m_Capacity), m_Size(other.m_Size), m_Data(other.m_Data) { other.m_Data = nullptr; }
  Vector(const Vector& other) : m_Capacity(other.m_Capacity), m_Size(other.m_Size) {
    m_Data = new T[other.m_Capacity];
    for (size_t i = 0; i < other.m_Size; i++) m_Data[i] = other.m_Data[i];
  }

  Vector(std::initializer_list<T> ilist) : m_Size(ilist.size()), m_Capacity(m_Size) {
    m_Data = new T[m_Capacity];
    size_t i = 0;
    for (auto& item : ilist) m_Data[i++] = item;
  }

  template <typename InputIt> Vector(InputIt first, InputIt last) : m_Size(last - first), m_Capacity(m_Size) {
    m_Data = new T[m_Capacity];
    for (auto i = first; i < last; i++) m_Data[i - first] = *i;
  }

  ~Vector() { delete[] m_Data; }

  Vector& operator=(Vector&& other) noexcept {
    if (this != &other) {
      delete[] m_Data;
      m_Data = other.m_Data;
      m_Size = other.m_Size;
      m_Capacity = other.m_Capacity;
      other.m_Data = nullptr;
    }
    return *this;
  }

  Vector& operator=(const Vector& other) {
    if (this != &other) {
      delete[] m_Data;
      m_Size = other.m_Size;
      m_Capacity = other.m_Capacity;
      m_Data = new T[other.m_Capacity];
      for (size_t i = 0; i < other.m_Size; i++) m_Data[i] = other.m_Data[i];
    }
    return *this;
  }

  // * Element Accsess
  T& at(size_t index) {
    assert(index < m_Size && "Index out of range!");
    return m_Data[index];
  }

  T& operator[](size_t index) { return at(index); }
  T& front() { return m_Data[0]; }
  T& back() { return m_Data[m_Size - 1]; }
  T* data() { return m_Data; }

  const T& at(size_t index) const {
    assert(index < m_Size && "Index out of range!");
    return m_Data[index];
  }

  const T& operator[](size_t index) const { return at(index); }
  const T& front() const { return m_Data[0]; }
  const T& back() const { return m_Data[m_Size - 1]; }
  const T* data() const { return m_Data; }

  // * Iterators
  template <typename pointer> struct ReverseIterator {
    pointer ptr = nullptr;
    ReverseIterator() = default;
    ReverseIterator(T* ptr) : ptr(ptr) {}

    // clang-format off
    ReverseIterator& operator++() { ptr--; return *this; }
    ReverseIterator operator++(int) { ReverseIterator tmp = *this; ptr--; return tmp; }
    ReverseIterator& operator--() { ptr++; return *this; }
    ReverseIterator operator--(int) { ReverseIterator tmp = *this; ptr++; return tmp; }
    bool operator==(const ReverseIterator& other) { return ptr == other.ptr; }
    bool operator!=(const ReverseIterator& other) { return ptr != other.ptr; }
    bool operator<(const ReverseIterator& other) { return ptr < other.ptr; }
    bool operator>(const ReverseIterator& other) { return ptr > other.ptr; }
    bool operator<=(const ReverseIterator& other) { return ptr <= other.ptr; }
    bool operator>=(const ReverseIterator& other) { return ptr >= other.ptr; }

    int operator-(const ReverseIterator& other) { return ptr - other.ptr; }
    ReverseIterator operator-(int i) { return ptr - i; }
    ReverseIterator operator+(int i) { return ptr + i; }

    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }
    // clang-format on
  };

  T* begin() { return m_Data; }
  T* end() { return m_Data + m_Size; }
  const T* cbegin() const { return m_Data; }
  const T* cend() const { return m_Data + m_Size; }
  ReverseIterator<T*> rbegin() { return {m_Data + m_Size - 1}; }
  ReverseIterator<T*> rend() { return {m_Data - 1}; }
  ReverseIterator<const T*> crbegin() const { return {m_Data + m_Size - 1}; }
  ReverseIterator<const T*> crend() const { return {m_Data - 1}; }

  // * Capacity
  bool empty() const { return m_Size == 0; }
  size_t size() const { return m_Size; }
  size_t capacity() const { return m_Capacity; }

  void reserve(size_t capacity) {
    T* newData = new T[capacity];
    memcpy(newData, m_Data, m_Size * sizeof(T));
    for (size_t i = m_Size; i < capacity; i++) newData[i] = T();
    delete[] m_Data;
    m_Data = newData;
    m_Capacity = capacity;
    m_Size = OreonContainer_min(m_Size, capacity);
  }

  void resize(size_t size) {
    reserve(size);
    m_Size = size;
  }

  void shrink_to_fit() { reserve(m_Size); }

  // * Modifiers
  void clear() { m_Size = 0; }
  T* insert(size_t pos, const T& value) {
    grow(pos, 1);
    m_Data[pos] = value;
    return begin() + pos;
  }

  T* insert(size_t pos, const Vector<T>& other) {
    grow(pos, other.size());
    for (size_t i = 0; i < other.size(); i++) m_Data[pos + i] = other[i];
    return begin() + pos;
  }

  template <typename InputIt> T* insert(size_t pos, InputIt first, InputIt last) {
    grow(pos, last - first);
    for (auto i = first; i != last; i++) m_Data[i - first] = *i;
    return begin() + pos;
  }

  T* insert(const T* pos, const T& value) { return insert(pos - begin(), value); }
  T* insert(const T* pos, const Vector<T>& other) { return insert(pos - begin(), other); }
  template <typename InputIt> T* insert(const T* pos, InputIt first, InputIt last) { return insert(pos - begin(), first, last); }

  void erase(size_t pos) {
    assert(pos < m_Size && "Index out of range!");
    assert(m_Size >= 1 && "Erasing more then size of the vector!");
    shrink(pos, 1);
  }

  void erase(size_t pos, size_t count) {
    assert(pos < m_Size && "Index out of range!");
    assert(pos + count <= m_Size && "Erasing more then size of the vector!");
    shrink(pos, count);
  }

  void erase(const T* pos) { erase(pos - begin()); }
  void erase(const T* first, const T* last) { erase(first - begin(), last - first); }
  T* push_back(const T& value) { return insert(m_Size, value); }
  void pop_back() { erase(m_Size - 1); }
  T* push_front(const T& value) { return insert(0, value); }
  void pop_front() { erase(0); }

private:
  void grow(size_t pos, size_t increment) {
    if (m_Size + increment > m_Capacity) reserve(OreonContainer_max(m_Capacity * 2, m_Size + increment));
    m_Size += increment;
    for (size_t i = m_Size - 1; i > pos; i--) m_Data[i] = m_Data[i - increment];
  }

  void shrink(size_t pos, size_t decrement) {
    m_Size -= decrement;
    for (size_t i = pos; i < m_Size; i++) m_Data[i] = m_Data[i + decrement];
  }

  T* m_Data;
  size_t m_Size, m_Capacity;
};

template <typename M, typename N> struct Pair {
  Pair() = default;
  Pair(M first, N second) : first(first), second(second) {}

  M first;
  N second;
};

template <typename K, typename V> class VectorMap : public Vector<Pair<K, V>> {
public:
  using Vector<Pair<K, V>>::at;
  using Vector<Pair<K, V>>::erase;
  using Vector<Pair<K, V>>::operator[];

  Pair<K, V>* find(const K& key) {
    for (auto& pair : *this) {
      if (pair.first == key) return &pair;
    }
    return Vector<Pair<K, V>>::end();
  }

  bool contains(const K& key) { return find(key) != Vector<Pair<K, V>>::end(); }

  V& at(const K& key) {
    for (auto& pair : *this) {
      if (pair.first == key) return pair.second;
    }
    Vector<Pair<K, V>>::push_back(Pair<K, V>(key, V()));
    return Vector<Pair<K, V>>::back().second;
  }

  void erase(const K& key) {
    auto it = Vector<Pair<K, V>>::begin();
    while (it != Vector<Pair<K, V>>::end()) {
      if (it->first == key) {
        Vector<Pair<K, V>>::erase(it);
        return;
      }
      it++;
    }
  }

  V& operator[](const K& key) { return at(key); }
};

// * Functions
template <typename InputIt, typename T> void for_each(InputIt first, InputIt last, void (*func)(T& value)) {
  for (auto i = first; i != last; i++) func(*i);
}

template <typename InputIt, typename T> void for_each(InputIt first, InputIt last, void (*func)(const T& value)) {
  for (auto i = first; i != last; i++) func(*i);
}

template <typename InputIt, typename T> void for_each(InputIt first, InputIt last, void (*func)(T& value, size_t index)) {
  size_t index = 0;
  for (auto i = first; i != last; i++) func(*i, index++);
}

template <typename InputIt, typename T> void for_each(InputIt first, InputIt last, void (*func)(const T& value, size_t index)) {
  size_t index = 0;
  for (auto i = first; i != last; i++) func(*i, index++);
}

template <typename T, typename E> void for_each(T& container, void (*func)(E& value)) { for_each(container.begin(), container.end(), func); }
template <typename T, typename E> void for_each(const T& container, void (*func)(const E& value)) { for_each(container.begin(), container.end(), func); }
} // namespace Container
