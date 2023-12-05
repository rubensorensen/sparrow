cmake_minimum_required(VERSION 3.20)

project(sparrow VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

Include(FetchContent)

# Get glfw
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
)
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# Get spdlog
FetchContent_Declare(
  spdlog
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
  glfw
  spdlog
)
if(MSVC)
  target_compile_options(sparrow PRIVATE /W4 /WX)
else()
  target_compile_options(sparrow PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Sparrow tests
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)
file(GLOB_RECURSE TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
add_executable(tests ${TEST_SRC_FILES} ${SPARROW_SRC_FILES})
target_include_directories(tests
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
)
target_link_libraries(tests
  PRIVATE
  Catch2::Catch2WithMain
  spdlog
)
