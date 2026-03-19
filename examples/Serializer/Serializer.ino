#include <OreonSerializer.hpp>

void setup() {
  Serial.begin(115200);
  LinkedList<uint8_t>* data = serialize("i1i2i4i8u1u2u4u8", -127, 32767, -2147483647, -9223372036854775808, 255, 65535, 4294967295, 18446744073709551615);
  int8_t i1;
  int16_t i2;
  int32_t i4;
  int64_t i8;
  uint8_t u1;
  uint16_t u2;
  uint32_t u4;
  uint64_t u8;
  deserialize("i1i2i4i8u1u2u4u8", data, &i1, &i2, &i4, &i8, &u1, &u2, &u4, &u8);
  Serial.print(i1); Serial.print(", ");
  Serial.print(i2); Serial.print(", ");
  Serial.print(i4); Serial.print(", ");
  Serial.print(i8); Serial.print(", ");
  Serial.print(u1); Serial.print(", ");
  Serial.print(u2); Serial.print(", ");
  Serial.print(u4); Serial.print(", ");
  Serial.println(u8);
  // -127, 32767, -2147483648, -9223372036854775808, 255, 65535, 4294967295, 18446744073709551615
  String s = "Hi";
  LinkedList<uint8_t> anotherData; // Could come from serialize
  anotherData.add(8);
  anotherData.add(5);
  data = serialize("sl", &s, &anotherData); // You can use 'S' and 'L', and input data will be deleted, protection from memory leaks
  s = "";
  anotherData.clear();
  deserialize("sl", data, &s, &anotherData);
  Serial.println(s);
  Serial.println(anotherData[0]);
  Serial.println(anotherData[1]);
}

void loop() {}
