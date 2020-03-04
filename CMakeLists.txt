cmake_minimum_required(VERSION 3.14)
project(util LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include(dependencies/BoostLoader.cmake)
BoostLoader_load(${PROJECT_SOURCE_DIR}/dependencies boost_1_72_0)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_subdirectory(test-executable)
endif()