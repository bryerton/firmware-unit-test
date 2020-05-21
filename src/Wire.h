#ifndef WIRE_H_
#define WIRE_H_

#include <stdint.h>
#include <catch.hpp>
#include <catch2/trompeloeil.hpp>
#include <Arduino.h>

class TwoWire {
public:
  static constexpr bool trompeloeil_movable_mock = true;

  // no return
  MAKE_MOCK1(beginTransmission, void(uint8_t));

  // endTransmission possible return codes
  // 0:success
  // 1:data too long to fit in transmit buffer
  // 2:received NACK on transmit of address
  // 3:received NACK on transmit of data
  // 4:other error
  MAKE_MOCK0(endTransmission, uint8_t(void));

  // endTransmission with stop parameter
  // true = send stop
  // false = send restart
  MAKE_MOCK1(endTransmission, uint8_t(bool));


  // returns the number of bytes returned from the slave device
  MAKE_MOCK2(requestFrom, uint8_t(uint8_t, uint8_t));
  MAKE_MOCK3(requestFrom, uint8_t(uint8_t, uint8_t, bool));

  // single byte read
  // returns value read (no error!)
  MAKE_MOCK0(read, uint8_t(void));

  // Single byte
  // returns number of bytes written
  MAKE_MOCK1(write, uint8_t(uint8_t));

  // data array and length
  // returns number of bytes written
  MAKE_MOCK2(write, uint8_t(uint8_t*,uint8_t));
};

extern TwoWire Wire;

#endif // WIRE_H_
