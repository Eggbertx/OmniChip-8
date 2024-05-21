cmake_minimum_required(VERSION 3.14)
project(oc8-test VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-fprofile-arcs -ftest-coverage)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")

include(CTest)
include(CTestCoverageCollectGCOV)
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	DOWNLOAD_EXTRACT_TIMESTAMP true
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCMAKE_TEST -fprofile-arcs -ftest-coverage")
set(OBJECT_DIR ${CMAKE_BINARY_DIR}/CMakeFiles/oc8-test.dir/src)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(oc8-test
	src/opcode_funcs.c
	src/chip8.c
	src/util.c
	src/io_template.c
	src/tests/chip8_test.cc
)

add_custom_target(lcov
	COMMAND rm -rf lcov
	COMMAND mkdir lcov
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_command(TARGET lcov
	COMMAND lcov  --ignore-errors inconsistent,mismatch,unused -d ${OBJECT_DIR} -c -o ${CMAKE_BINARY_DIR}/lcov/coverage.info -rc lcov_branch_coverage=1
	COMMAND lcov --ignore-errors inconsistent,unused -r ${CMAKE_BINARY_DIR}/lcov/coverage.info "/usr/include/*" "${CMAKE_BINARY_DIR}/_deps/*" -o ${CMAKE_BINARY_DIR}/lcov/coverage.info
	COMMAND genhtml ${CMAKE_BINARY_DIR}/lcov/coverage.info --branch-coverage --output-directory ${CMAKE_BINARY_DIR}/lcov/html
)

add_test(oc8-test ${CMAKE_CURRENT_BINARY_DIR}/oc8-test)

target_link_libraries(oc8-test GTest::gtest_main -fprofile-arcs)

include(GoogleTest)
gtest_discover_tests(oc8-test)

