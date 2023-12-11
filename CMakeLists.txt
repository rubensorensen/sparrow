cmake_minimum_required(VERSION 3.20)

project(sparrow VERSION 0.0.1 LANGUAGES C)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

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


set(SPARROW_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SPARROW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SPARROW_SRC_FILES ${SPARROW_SRC_DIR}/*.c)
file(GLOB_RECURSE SPARROW_HEADER_FILES ${SPARROW_INCLUDE_DIR}/*.h)

add_executable(sparrow
  ${SPARROW_SRC_FILES}
  ${SPARROW_HEADER_FILES}
)

target_include_directories(sparrow
  PRIVATE
  ${SPARROW_INCLUDE_DIR}
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

add_subdirectory(tests)
