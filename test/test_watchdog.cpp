#include <Arduino.h>
#include <I2C.h>
#include <Watchdog.h>
#include <Wire.h>

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

TEST_CASE("Watchdog Succeed", "[driver][watchdog]") {
  I2C i2c(Wire);
  trompeloeil::sequence seq;

  // Why 1E6? Because the watchdog is hardcoded (at the moment) to use that as it's timeout period
  REQUIRE_CALL(arduino_mock, millis()).RETURN(0).IN_SEQUENCE(seq); // AsyncDelay(d,u) constructor

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint8_t), true))
      .RETURN(sizeof(uint8_t)).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, read()).RETURN(0xAA).IN_SEQUENCE(seq);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  REQUIRE_CALL(arduino_mock, millis()).RETURN(0).IN_SEQUENCE(seq); // AsyncDelay.start()
  REQUIRE_CALL(arduino_mock, millis()).RETURN(1E6).IN_SEQUENCE(seq); // expire check

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint8_t), true))
      .RETURN(sizeof(uint8_t)).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, read()).RETURN(0xAA).IN_SEQUENCE(seq);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  REQUIRE_CALL(arduino_mock, millis()).RETURN(2E6).IN_SEQUENCE(seq); // restart check


  // On startup watchdog should be OK
  Watchdog wd(i2c, 0x10);
  REQUIRE(wd.status() == 0);
  REQUIRE(wd.ok() == true);

  // Start watchdog
  wd.begin();
  wd.loop();
  REQUIRE( wd.status() == 0);
}

TEST_CASE("Watchdog Fail on I2C read", "[driver][watchdog]") {
  I2C i2c(Wire);
  trompeloeil::sequence seq;

  // Why 1E6? Because the watchdog is hardcoded (at the moment) to use that as it's timeout period
  REQUIRE_CALL(arduino_mock, millis()).RETURN(0).IN_SEQUENCE(seq); // AsyncDelay(d,u) constructor

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint8_t), true))
      .RETURN(sizeof(uint8_t)).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, read()).RETURN(0xAA).IN_SEQUENCE(seq);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  REQUIRE_CALL(arduino_mock, millis()).RETURN(0).IN_SEQUENCE(seq); // AsyncDelay.start()
  REQUIRE_CALL(arduino_mock, millis()).RETURN(1E6).IN_SEQUENCE(seq); // expire check

  // Fail here by returning a bad length from requestFrom (0)
  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint8_t), true))
      .RETURN(0).IN_SEQUENCE(seq);
  // Due to the requestFrom failing, the following never occurs
  /*
  REQUIRE_CALL(Wire, read()).RETURN(0xAA).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);
  */
  REQUIRE_CALL(arduino_mock, millis()).RETURN(2E6).IN_SEQUENCE(seq); // restart check

  // On startup watchdog should be OK
  Watchdog wd(i2c, 0x10);
  REQUIRE(wd.status() == 0);
  REQUIRE(wd.ok() == true);

  // Start watchdog
  wd.begin();
  wd.loop();
  REQUIRE( wd.status() != 0);
}

TEST_CASE("Watchdog Fail on I2C write", "[driver][watchdog]") {
  I2C i2c(Wire);
  trompeloeil::sequence seq;

  // Why 1E6? Because the watchdog is hardcoded (at the moment) to use that as it's timeout period
  REQUIRE_CALL(arduino_mock, millis()).RETURN(0).IN_SEQUENCE(seq); // AsyncDelay(d,u) constructor

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint8_t), true))
      .RETURN(sizeof(uint8_t)).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, read()).RETURN(0xAA).IN_SEQUENCE(seq);

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  REQUIRE_CALL(arduino_mock, millis()).RETURN(0).IN_SEQUENCE(seq); // AsyncDelay.start()
  REQUIRE_CALL(arduino_mock, millis()).RETURN(1E6).IN_SEQUENCE(seq); // expire check

  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), sizeof(uint8_t), true))
      .RETURN(sizeof(uint8_t)).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, read()).RETURN(0xAA).IN_SEQUENCE(seq);

  // Fail here by returning non-zero from endTransmission
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(ANY(uint8_t))).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(1).IN_SEQUENCE(seq);

  REQUIRE_CALL(arduino_mock, millis()).RETURN(2E6).IN_SEQUENCE(seq); // restart check

  REQUIRE_CALL(arduino_mock, millis()).RETURN(3E6).IN_SEQUENCE(seq); // restart check
  REQUIRE_CALL(arduino_mock, millis()).RETURN(4E6).IN_SEQUENCE(seq); // restart check

  // On startup watchdog should be OK
  Watchdog wd(i2c, 0x10);
  REQUIRE(wd.status() == 0);
  REQUIRE(wd.ok() == true);

  // Start watchdog
  wd.begin();
  wd.loop();
  REQUIRE( wd.status() != 0);
  // Go one more loop through to get another branch path in _io()
  wd.loop();
  REQUIRE( wd.status() != 0);
}

