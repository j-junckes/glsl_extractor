cmake_minimum_required(VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(ANTLR4_VERSION "4.13.1" CACHE STRING "Specify the version of ANTLR4 to download")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
  set(GIT_TAG_NAME "v0.0.0")
endif ()

string(SUBSTRING ${GIT_TAG_NAME} 1 -1 PROJECT_CMAKE_VERSION_FROM_GIT)

project(glsl_extractor VERSION ${PROJECT_CMAKE_VERSION_FROM_GIT} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_compile_definitions(GLSL_EXTRACTOR_VERSION="${GIT_TAG_NAME}")
add_compile_definitions(GLSL_EXTRACTOR_COMMIT="${GIT_COMMIT_HASH}")
add_compile_definitions(GLSL_EXTRACTOR_BRANCH="${GIT_BRANCH_NAME}")
add_compile_definitions(GLSL_EXTRACTOR_COMPILER="${CMAKE_CXX_COMPILER_ID}")

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

set(ANTLR4_JAR_NAME "antlr-${ANTLR4_VERSION}-complete.jar")
set(ANTLR4_JAR_URL "https://www.antlr.org/download/${ANTLR4_JAR_NAME}")
set(ANTLR4_JAR_PATH "${CMAKE_BINARY_DIR}/antlr4/${ANTLR4_JAR_NAME}")

if (NOT EXISTS "${ANTLR4_JAR_PATH}")
  message(STATUS "Downloading ANTLR4 ${ANTLR4_VERSION} from ${ANTLR4_JAR_URL}")

  file(DOWNLOAD
    "${ANTLR4_JAR_URL}"
    "${ANTLR4_JAR_PATH}"
    SHOW_PROGRESS
    STATUS download_status
    LOG download_log)
  list(GET download_status 0 status_code)
  if (status_code)
    message(FATAL_ERROR "Error downloading ANTLR4 jar: ${download_log}")
  endif ()
else ()
  message(STATUS "Using cached ANTLR4 ${ANTLR4_VERSION} from ${ANTLR4_JAR_PATH}")
endif ()

set(ANTLR_EXECUTABLE ${ANTLR4_JAR_PATH})

include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})

find_package(ANTLR REQUIRED)

antlr_target(GLSLLexer ${CMAKE_CURRENT_SOURCE_DIR}/parser/GLSLLexer.g4 LEXER
  PACKAGE GLSL
)

antlr_target(GLSLParser ${CMAKE_CURRENT_SOURCE_DIR}/parser/GLSLParser.g4 PARSER
  PACKAGE GLSL
  DEPENDS_ANTLR GLSLLexer
  COMPILE_FLAGS -lib ${ANTLR_GLSLLexer_OUTPUT_DIR}
  LISTENER
  VISITOR
)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_library(glsl_extractor_lib
  ${ANTLR_GLSLLexer_CXX_OUTPUTS}
  ${ANTLR_GLSLParser_CXX_OUTPUTS}
  src/GLSLExporterVisitor.cpp
  src/GLSLExtractor.cpp
)

target_include_directories(glsl_extractor_lib PRIVATE
  ${ANTLR_GLSLLexer_OUTPUT_DIR}
  ${ANTLR_GLSLParser_OUTPUT_DIR}
  ${ANTLR4_INCLUDE_DIRS}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(glsl_extractor_lib PRIVATE
  antlr4_static
  nlohmann_json::nlohmann_json
)

install(TARGETS glsl_extractor_lib EXPORT glsl_extractor_libTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY ${GLSLParser_INCLUDE_DIRS}/
  DESTINATION include
)

#install(EXPORT glsl_extractor_libTargets
#  FILE glsl_extractor_libConfig.cmake
#  NAMESPACE glsl_extractor_lib::
#  DESTINATION lib/cmake/glsl_extractor_lib
#)

#export(EXPORT glsl_extractor_libTargets
#  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/glsl_extractor_libConfig.cmake"
#  NAMESPACE glsl_extractor_lib::
#)

set_target_properties(glsl_extractor_lib PROPERTIES OUTPUT_NAME "glslextractor")

add_compile_definitions(GLSL_EXTRACTOR_WITH_NLOHMANN_JSON)

add_executable(glsl_extractor
  src/main.cpp
)

target_include_directories(glsl_extractor PRIVATE
  ${THIRD_PARTY_DIR}/argparse/include
)

target_link_libraries(glsl_extractor PRIVATE
  glsl_extractor_lib
  nlohmann_json::nlohmann_json
)

install(TARGETS glsl_extractor
  RUNTIME DESTINATION bin
)

set_target_properties(glsl_extractor PROPERTIES OUTPUT_NAME "glslextractor")

install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX})

