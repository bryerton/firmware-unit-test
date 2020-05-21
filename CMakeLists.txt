cmake_minimum_required(VERSION 3.5)

project(mvm_unittests LANGUAGES CXX VERSION 0.1.0)

add_subdirectory(thirdparty/Catch2)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/Catch2/contrib")

set(GCOV_LINK_FLAGS "-lgcov --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCOV_LINK_FLAGS}")
set(CMAKE_CXX_FLAGS "--coverage")

add_executable(mvm_unittests
  src/drivers/src/ADS1115.cpp
  src/drivers/src/AsyncIO.cpp
  #src/drivers/src/GPI.cpp
  #src/drivers/src/GPO.cpp
  src/drivers/src/I2C.cpp
  src/drivers/src/M24C02.cpp
  src/drivers/src/MS5525DSO.cpp
  src/drivers/src/NPA700.cpp
  src/drivers/src/SFM3x.cpp
  src/drivers/src/SWCRC.cpp
  src/drivers/src/TCA9548A.cpp
  src/drivers/src/Watchdog.cpp

  test/test_asyncio.cpp
  test/test_i2c.cpp
  test/test_tca9548a.cpp
  test/test_watchdog.cpp
  test/test_swcrc.cpp
  test/test_m24c02.cpp

  # Test runner entry point
  test/main.cpp

  # Arduino CRC32 library
  src/CRC32.cpp
)

include_directories(
  "thirdparty/trompeloeil/include"
  "src"
  "src/drivers/src"
)

target_compile_features(mvm_unittests PUBLIC cxx_std_14)
target_link_libraries(mvm_unittests Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(mvm_unittests)
