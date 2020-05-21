#include <Arduino.h>
#include <I2C.h>
#include <M24C02.h>
#include <Wire.h>

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

TEST_CASE("M24C02 Read Success", "[driver][m24c02]") {
  I2C i2c(Wire);
  M24C02 eeprom(i2c);
  uint8_t v;
  bool stop_bit = true;

  trompeloeil::sequence seq;

  for(uint32_t n=0; n<256; n++) {
    uint8_t expected_byte = n ^ 0xAE; // just jumbling it up a bit
    REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
    REQUIRE_CALL(Wire, write(n)).RETURN(1).IN_SEQUENCE(seq);
    REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

    REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), 1, stop_bit)).RETURN(1).IN_SEQUENCE(seq);
    REQUIRE_CALL(Wire, read()).RETURN(expected_byte).IN_SEQUENCE(seq);

    // @BUG: The datasheet for the M24C02 specifies no wait here, there is a only a 5ms wait on writes
    // So this delay call inside the driver is unnecessary
    ALLOW_CALL(arduino_mock, delay(ANY(uint32_t))).IN_SEQUENCE(seq);

    v = eeprom.read(n);
    REQUIRE(v == expected_byte);
    REQUIRE( eeprom.status() == 0);
  }
}

TEST_CASE("M24C02 Read Failure on write", "[driver][m24c02]") {
  I2C i2c(Wire);
  M24C02 eeprom(i2c);
  bool stop_bit = true;

  trompeloeil::sequence seq;

  uint32_t n = 0;
  uint8_t expected_byte = n ^ 0xAE; // just jumbling it up a bit

  // Fail out on endTransmission by returning a non-zero value
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(n)).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(2).IN_SEQUENCE(seq);

  // @BUG: The driver should stop if the write fails and not continue on
  ALLOW_CALL(Wire, requestFrom(ANY(uint8_t), 1, stop_bit)).RETURN(1).IN_SEQUENCE(seq);
  ALLOW_CALL(Wire, read()).RETURN(expected_byte).IN_SEQUENCE(seq);

  // @BUG: The datasheet for the M24C02 specifies no wait here, there is a only a 5ms wait on writes
  // So this delay call inside the driver is unnecessary
  ALLOW_CALL(arduino_mock, delay(ANY(uint32_t))).IN_SEQUENCE(seq);

  eeprom.read(n);
  REQUIRE( eeprom.status() != 0);
}

TEST_CASE("M24C02 Read Failure on read", "[driver][m24c02]") {
  I2C i2c(Wire);
  M24C02 eeprom(i2c);
  bool stop_bit = true;

  trompeloeil::sequence seq;

  uint32_t n = 0;
  uint8_t expected_byte = n ^ 0xAE; // just jumbling it up a bit
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(n)).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  // Return a bad read length from requestFrom()
  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), 1, stop_bit)).RETURN(0).IN_SEQUENCE(seq);
  // REQUIRE_CALL(Wire, read()).RETURN(expected_byte).IN_SEQUENCE(seq);

  // @BUG: The datasheet for the M24C02 specifies no wait here, there is a only a 5ms wait on writes
  // So this delay call inside the driver is unnecessary
  ALLOW_CALL(arduino_mock, delay(ANY(uint32_t))).IN_SEQUENCE(seq);

  eeprom.read(n);
  REQUIRE( eeprom.status() != 0);

  // Subsequent read returns immediately
  eeprom.read(n);
}

TEST_CASE("M24C02 Write Success", "[driver][m24c02]") {
  I2C i2c(Wire);
  M24C02 eeprom(i2c);
  bool stop_bit = true;

  trompeloeil::sequence seq;

  for(uint32_t n=0; n<256; n++) {
    uint8_t expected_byte = n ^ 0xAE; // just jumbling it up a bit
    REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
    REQUIRE_CALL(Wire, write(n)).RETURN(1).IN_SEQUENCE(seq);
    REQUIRE_CALL(Wire, write(expected_byte)).RETURN(1).IN_SEQUENCE(seq);
    REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

    // @BUG: Wait appears to be twice as long as necessary (10 vs 5)
    ALLOW_CALL(arduino_mock, delay(ANY(uint32_t))).IN_SEQUENCE(seq);

    eeprom.write(n, expected_byte);
    REQUIRE( eeprom.status() == 0);
  }
}

TEST_CASE("M24C02 Write Failure", "[driver][m24c02]") {
  I2C i2c(Wire);
  M24C02 eeprom(i2c);
  bool stop_bit = true;

  trompeloeil::sequence seq;

  uint32_t n = 0;
  uint8_t expected_byte = n ^ 0xAE; // just jumbling it up a bit

  // fail by returning non-zero on endTransmission()
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(n)).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(expected_byte)).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(3).IN_SEQUENCE(seq);

  // @BUG: There doesn't need to be a delay if the write failed
  ALLOW_CALL(arduino_mock, delay(ANY(uint32_t))).IN_SEQUENCE(seq);

  eeprom.write(n, expected_byte);
  REQUIRE( eeprom.status() != 0);

  // Subsequent write returns immediately
  eeprom.write(n, expected_byte);
}

