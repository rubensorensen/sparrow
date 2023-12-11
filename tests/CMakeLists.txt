cmake_minimum_required(VERSION 3.20)

project(sparrow_tests VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

set(TEST_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE TEST_SRC_FILES ${TEST_SRC_DIR}/*.cpp)
set(SPARROW_SRC_FILES_WITHOUT_MAIN ${SPARROW_SRC_FILES})
list(REMOVE_ITEM  SPARROW_SRC_FILES_WITHOUT_MAIN ${SPARROW_SRC_DIR}/main.c)

add_executable(test
  ${TEST_SRC_FILES}
  ${SPARROW_SRC_FILES_WITHOUT_MAIN}
)

target_include_directories(test
  PRIVATE
  ${SPARROW_INCLUDE_DIR}
)

target_link_libraries(test
  PRIVATE
  Catch2::Catch2WithMain
)
