cmake_minimum_required(VERSION 3.20)

project(sparrow VERSION 0.0.1 LANGUAGES C)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

# Sparrow executable
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE INC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

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
)

if(MSVC)
  target_compile_options(sparrow PRIVATE /W4 /WX)
else()
  target_compile_options(sparrow PRIVATE -Wall -Wextra -Wpedantic -ggdb -O0)
endif()

# Sparrow tests
# FetchContent_Declare(
  # Catch2
  # GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  # GIT_TAG        v3.4.0
# )
# FetchContent_MakeAvailable(Catch2)

# file(GLOB_RECURSE TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
# file(GLOB_RECURSE SPARROW_SRC_FILES_WITHOUT_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# list(REMOVE_ITEM  SPARROW_SRC_FILES_WITHOUT_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# add_executable(test
  # ${TEST_SRC_FILES}
  # ${SPARROW_SRC_FILES_WITHOUT_MAIN}
# )

# target_include_directories(test
  # PRIVATE
  # ${CMAKE_CURRENT_SOURCE_DIR}/include/
# )

# target_link_libraries(test
  # PRIVATE
  # Catch2::Catch2WithMain
# )
