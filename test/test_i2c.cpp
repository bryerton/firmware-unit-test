#include <I2C.h>

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

TEST_CASE("I2C Writes succeed", "[driver][i2c]") {
  I2C i2c(Wire);

  for (uint32_t n = 0; n < 256; n++) {
    REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
    REQUIRE_CALL(Wire, write(n)).RETURN(1);
    REQUIRE_CALL(Wire, endTransmission()).RETURN(0);
    REQUIRE(i2c.write1(0x00, n) == 0);
  }

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
  REQUIRE_CALL(Wire, write(0xAA)).RETURN(1);
  REQUIRE_CALL(Wire, write(0xBB)).RETURN(1);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0);

  REQUIRE(i2c.write2(0x00, 0xAABB) == 0);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
  REQUIRE_CALL(Wire, write(0xAA)).RETURN(1);
  REQUIRE_CALL(Wire, write(0xBB)).RETURN(1);
  REQUIRE_CALL(Wire, write(0xCC)).RETURN(1);
  REQUIRE_CALL(Wire, write(0xDD)).RETURN(1);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0);

  REQUIRE(i2c.write4(0x00, 0xAABBCCDD) == 0);
}

/*
Test for failure return on i2c.writeX calls

There is no formal definition for the return value of i2c.writeX,
for now we will just check for non-zero return

@TODO: Get formal definition of return values expected from i2c.writeX calls

*/
TEST_CASE("I2C Writes report failure", "[driver][i2c]") {
  I2C i2c(Wire);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(0);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(1);
  REQUIRE(i2c.write1(0x00, 0xAA) != 0);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(0);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(2);
  REQUIRE(i2c.write1(0x00, 0xAA) != 0);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(0);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(3);
  REQUIRE(i2c.write1(0x00, 0xAA) != 0);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(0);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(4);
  REQUIRE(i2c.write1(0x00, 0xAA) != 0);

  // This should *not* crash, but currently segfaults
  /*
  // @BUG: No check for NULL in i2c.write()
  ALLOW_CALL(Wire, beginTransmission(ANY(uint8_t)));
  ALLOW_CALL(Wire, write(ANY(uint8_t))).RETURN(0);
  ALLOW_CALL(Wire, endTransmission()).RETURN(0);
  REQUIRE( i2c.write(0x00, 0, 4) == 0);
  */
}

TEST_CASE("I2C Reads succeed", "[driver][i2c]") {
  I2C i2c(Wire);
  bool stop_bit = true;
  uint8_t d_byte;
  uint8_t expected_byte = 0xAA;

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), 1, stop_bit)).RETURN(1);
  REQUIRE_CALL(Wire, read()).RETURN(expected_byte);

  REQUIRE(i2c.read1(0x00, d_byte) == 0);
  REQUIRE(d_byte == expected_byte);

  // 2-byte read
  uint16_t d16;
  uint16_t expected16 = 0xAABB;

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint16_t), stop_bit))
      .RETURN(sizeof(uint16_t));
  REQUIRE_CALL(Wire, read()).RETURN(0xAA);
  REQUIRE_CALL(Wire, read()).RETURN(0xBB);

  REQUIRE(i2c.read2(0x00, d16) == 0);
  REQUIRE(d16 == expected16);

  // 4-byte read
  uint32_t d32;
  uint32_t expected32 = 0xAABBCCDD;

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint32_t), stop_bit))
      .RETURN(sizeof(uint32_t));
  REQUIRE_CALL(Wire, read()).RETURN(0xAA);
  REQUIRE_CALL(Wire, read()).RETURN(0xBB);
  REQUIRE_CALL(Wire, read()).RETURN(0xCC);
  REQUIRE_CALL(Wire, read()).RETURN(0xDD);

  REQUIRE(i2c.read4(0x00, d32) == 0);
  REQUIRE(d32 == expected32);
}

TEST_CASE("I2C Reads report failure", "[driver][i2c]") {
  I2C i2c(Wire);

  // We are sending a stop bit on every i2c read
  bool stop_bit = true;

  uint32_t n;

  // 1-byte read
  uint8_t d_byte;

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), 1, stop_bit)).RETURN(0);
  ALLOW_CALL(Wire, read()).RETURN(0xAA);
  REQUIRE(i2c.read1(0x00, d_byte) != 0);

  // 2-byte read
  uint16_t d16;

  for (n = 0; n < 2; n++) {
    REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint16_t), stop_bit))
        .RETURN(n);
    ALLOW_CALL(Wire, read()).RETURN(0xAA);
    ALLOW_CALL(Wire, read()).RETURN(0xBB);
    REQUIRE(i2c.read2(0x00, d16) != 0);
  }

  // 4-byte read
  uint32_t d32;

  for (n = 0; n < 3; n++) {
    REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint32_t), stop_bit))
        .RETURN(3);
    ALLOW_CALL(Wire, read()).RETURN(0xAA);
    ALLOW_CALL(Wire, read()).RETURN(0xBB);
    ALLOW_CALL(Wire, read()).RETURN(0xCC);
    ALLOW_CALL(Wire, read()).RETURN(0xDD);

    REQUIRE(i2c.read4(0x00, d32) != 0);
  }

  // This should *not* crash, but currently 'succeeds' when it should segfault
  // due to a lack of check for NULL pointer being passed to i2c.read()
  // @BUG: No check for NULL in i2c.write()
  /*
  ALLOW_CALL(Wire, requestFrom(ANY(uint8_t), ANY(uint8_t), stop_bit)).RETURN(0);
  ALLOW_CALL(Wire, read()).RETURN(0x00);
  REQUIRE( i2c.read(0x00, 0, 4) != 0);
  */
}
