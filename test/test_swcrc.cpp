#include <SWCRC.h>
#include <stdint.h>

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

uint8_t *_text_start;
uint8_t *_text_end;

static uint8_t text[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xAA, 0xBB, 0xCC, 0xDD};

TEST_CASE("SWCRC", "[driver][swcrc]") {
  // The assumption is these are passed in via the linker, and represent some
  // file (the executable?)
  _text_start = &text[0];
  _text_end = &text[sizeof(text) - 1];

  REQUIRE_CALL(arduino_mock, millis()).RETURN(0).TIMES(AT_LEAST(1));

  SWCRC crc(10, 4);

  // @TODO: Implement the test cases for SWCRC
}
