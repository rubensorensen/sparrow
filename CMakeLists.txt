cmake_minimum_required(VERSION 3.20)

project(sparrow VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

Include(FetchContent)

# Get SDL
set(SDL_SHARED false)
set(SDL_STATIC true)
FetchContent_Declare(SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG main
)
FetchContent_MakeAvailable(SDL)
  
# Get spdlog
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)

# Sparrow executable
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE INC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_executable(sparrow
  ${SRC_FILES}
  ${INC_FILES}
)

target_include_directories(sparrow
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(sparrow
  PRIVATE
  SDL3::SDL3-static
  spdlog
)

if(MSVC)
  target_compile_options(sparrow PRIVATE /W4 /WX)
else()
  target_compile_options(sparrow PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Sparrow tests
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
file(GLOB_RECURSE SPARROW_SRC_FILES_WITHOUT_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM  SPARROW_SRC_FILES_WITHOUT_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(test
  ${TEST_SRC_FILES}
  ${SPARROW_SRC_FILES_WITHOUT_MAIN}
)

target_include_directories(test
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(test
  PRIVATE
  Catch2::Catch2WithMain
  spdlog
)
