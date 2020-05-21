#ifndef ARDUINO_H_
#define ARDUINO_H_

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

class ArduinoMock {
 public:
  MAKE_MOCK0(millis, int64_t());
  MAKE_MOCK0(micros, int64_t());
  MAKE_MOCK1(delay, void(uint32_t));
};

extern ArduinoMock arduino_mock;

extern "C" {
int64_t millis();
int64_t micros();
void delay(uint32_t ms);
}

#endif  // ARDUINO_H_
