cmake_minimum_required(VERSION 3.14)
project(oc8-test VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
include(CTestCoverageCollectGCOV)
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCMAKE_TEST")
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(oc8-test
	src/chip8.c
	src/util.c
	src/io_template.c
	src/tests/chip8_test.cc
)

target_link_libraries(oc8-test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(oc8-test)
