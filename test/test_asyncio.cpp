#include <Arduino.h>
#include <AsyncIO.h>

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

// AsyncIO is a virtual class, so we have to inherit it into a test class
class AsyncIOTest : public AsyncIO {
 public:
  AsyncIOTest(float us_min) : AsyncIO(us_min){ _start(); };
  bool _io() { return true; };
};

TEST_CASE("AsyncIO Tests", "[driver][asyncio]") {
  REQUIRE_CALL(arduino_mock, millis()).RETURN(10).TIMES(AT_LEAST(1));
  REQUIRE_CALL(arduino_mock, micros()).RETURN(10).TIMES(AT_LEAST(1));

  AsyncIOTest async_test(0.0f);
  async_test.loop();

  AsyncIOTest async_test2(10E3);
  async_test2.loop();

  //@TODO: Add tests and check branches (isExpired etc)
}
