cmake_minimum_required(VERSION 3.26)

find_package(Git REQUIRED)

execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH_NAME
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TAG_NAME
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (GIT_TAG_NAME STREQUAL "")
  set(GIT_TAG_NAME "0.0.0")
endif()

project(glsl_to_cpp VERSION ${GIT_TAG_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_compile_definitions(GLSL_TO_CPP_VERSION="${GIT_TAG_NAME}")
add_compile_definitions(GLSL_TO_CPP_COMMIT="${GIT_COMMIT_HASH}")
add_compile_definitions(GLSL_TO_CPP_BRANCH="${GIT_BRANCH_NAME}")
add_compile_definitions(GLSL_TO_CPP_COMPILER="${CMAKE_CXX_COMPILER_ID}")

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

add_executable(glsl_to_cpp
  src/main.cpp
)

target_include_directories(glsl_to_cpp PRIVATE
  ${THIRD_PARTY_DIR}/argparse/include
)
