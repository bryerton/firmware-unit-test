#include <TCA9548A.h>

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

TEST_CASE("TCA9548A Select", "[driver][tca9548a]") {
  TCA9548A tca(Wire);
  uint8_t switch_delay = 127;
  uint8_t sel_ch_bit = 7;

  // just calling it to make sure it doesn't segfault...
  // it's unclear why the TCA9548 can be passed a
  // variable delay period. The switching delay is
  // not variable.

  tca.begin(switch_delay);

  // @BUG: No check for success possible

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
  REQUIRE_CALL(Wire, write(1 << sel_ch_bit)).RETURN(1);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0);
  REQUIRE_CALL(arduino_mock, delay(switch_delay));
  tca.select(sel_ch_bit);  // select the channel we intend to use
  tca.select(
      sel_ch_bit);  // re-select the channel to get the quick-return branch

  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t)));
  REQUIRE_CALL(Wire, write(0)).RETURN(1);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0);
  REQUIRE_CALL(arduino_mock, delay(switch_delay));

  tca.select(-1);  // unselect all channels

  tca.select(
      8);  // illegal selection, falls through select() without i2c bus call
}

TEST_CASE("I2C_TCA9548A Select Same Channel", "[driver][tca9548a]") {
  uint8_t sw0_ch = 2;
  uint8_t sw1_ch = 4;
  uint8_t test_data[] = {0xAA};
  uint8_t expected_data[] = {0xAA};
  uint8_t switch_delay = 0;  // no delay call required if its zero
  bool stop_bit = true;
  TCA9548A tca(Wire);
  I2C_TCA9548A sw0(tca, sw0_ch);
  I2C_TCA9548A sw1(tca, sw1_ch);

  tca.begin(switch_delay);

  trompeloeil::sequence seq;

  // TCA9548 select()
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(1 << sw0_ch)).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  // Underlying I2C write
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(test_data[0])).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  // No TCA9548 select(), same channel

  // Underlying I2C read
  REQUIRE_CALL(Wire, requestFrom(ANY(uint8_t), 1, stop_bit))
      .RETURN(1)
      .IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, read()).RETURN(expected_data[0]).IN_SEQUENCE(seq);

  sw0.write(0x01, test_data, sizeof(test_data));
  sw0.read(0x01, test_data, sizeof(test_data));
}

TEST_CASE("I2C_TCA9548A Select different Channel", "[driver][tca9548a]") {
  uint8_t sw0_ch = 2;
  uint8_t sw1_ch = 4;
  uint8_t test_data[] = {0xAA};
  uint8_t expected_data[] = {0xAA};
  uint8_t switch_delay = 0;  // no delay call required if its zero
  bool stop_bit = true;
  TCA9548A tca(Wire);
  I2C_TCA9548A sw0(tca, sw0_ch);
  I2C_TCA9548A sw1(tca, sw1_ch);

  tca.begin(switch_delay);

  trompeloeil::sequence seq;
  // TCA9548 select()
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(1 << sw1_ch)).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  // Underlying I2C write
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(test_data[0])).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  // TCA9548 select()
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(1 << sw0_ch)).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  // Underlying I2C write
  REQUIRE_CALL(Wire, beginTransmission(ANY(uint8_t))).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, write(test_data[0])).RETURN(1).IN_SEQUENCE(seq);
  REQUIRE_CALL(Wire, endTransmission()).RETURN(0).IN_SEQUENCE(seq);

  sw1.write(0x01, test_data, sizeof(test_data));
  sw0.write(0x01, test_data, sizeof(test_data));
}
