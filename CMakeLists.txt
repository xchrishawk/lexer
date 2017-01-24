#
# CMakeLists.txt
# Chris Vig (chris@invictus.so)
#

# -- Project Configuration --

cmake_minimum_required(VERSION 2.8.7)
project(lexer)

# Targets
set(MAIN_TARGET ${CMAKE_PROJECT_NAME})
set(TESTS_TARGET ${CMAKE_PROJECT_NAME}_tests)

# Directories
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Toolchain configuration
set(CMAKE_CXX_FLAGS "-std=gnu++14 -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Werror -O2 -s")

# -- Third Party Libraries --

# Google Test (for unit testing)
find_package(GTest)

# -- Main Executable --

# Build main executable
add_executable(${MAIN_TARGET}
  ${SOURCE_DIR}/lexical_analyzer.cpp
  ${SOURCE_DIR}/main.cpp)
target_include_directories(${MAIN_TARGET}
  PRIVATE ${SOURCE_DIR})

# -- Tests Executable --

if (${GTEST_FOUND})

  # Builds tests executable
  add_executable(${TESTS_TARGET} EXCLUDE_FROM_ALL
    ${TESTS_DIR}/main.cpp)
  target_include_directories(${TESTS_TARGET}
    PRIVATE ${SOURCE_DIR}
    PRIVATE ${TESTS_DIR}
    PRIVATE ${GTEST_INCLUDE_DIRS})
  target_link_libraries(${TESTS_TARGET}
    ${GTEST_BOTH_LIBRARIES}
    pthread)

  # Run tests executable
  add_custom_target(runtests
    COMMAND ${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
    WORKING_DIRECTORY ${BUILD_DIR}
    COMMENT "Running ${CMAKE_PROJECT_NAME} unit tests...")

endif()
