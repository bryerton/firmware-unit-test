#define CATCH_CONFIG_RUNNER
#include <Arduino.h>
#include <Wire.h>

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

/*
trompeloeil::set_reporter([](trompeloeil::severity s, const char* file,
                             unsigned long line, std::string const& msg) {
  std::ostringstream os;
  if (line) os << file << ':' << line << '\n';
  os << msg;
  auto failure = os.str();
  if (s == severity::fatal) {
    // Must not return normally i.e. must throw, abort or terminate.
    TS_FAIL(failure);
  } else {
    // nonfatal: violation occurred during stack rollback.
    // Must not throw an exception.
    TS_WARN(failure);
  }
});
*/

// Used to help mock out millis() and micros() calls
ArduinoMock arduino_mock;

// Wire.h defines this, so we have to implement it somewhere
TwoWire Wire = TwoWire();

extern "C" {

int64_t micros(void) { return arduino_mock.micros(); }

int64_t millis(void) { return arduino_mock.millis(); }

void delay(uint32_t ms) { return arduino_mock.delay(ms); }
}

int main(int argc, char* argv[]) {
  // global setup...

  int result = Catch::Session().run(argc, argv);

  // global clean-up...

  return result;
}
