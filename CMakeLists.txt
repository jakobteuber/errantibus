# Errantibus, by Jakob Teuber
# See <https://github.com/jakobteuber/errantibus> 

cmake_minimum_required(VERSION 3.20)
project(Errantibus VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)


# ---------------------------------------------------------------------------
# Boost Stacktrace
# ---------------------------------------------------------------------------

find_package(Boost REQUIRED COMPONENTS stacktrace_basic stacktrace_addr2line)

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
    add_definitions(-DBOOST_STACKTRACE_USE_ADDR2LINE)
else()
    message(FATAL_ERROR "Boost.Stacktrace library not found!")
endif()

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
add_library(Errantibus STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/errantibus.cpp")
target_link_libraries(Errantibus PRIVATE Boost::stacktrace_basic Boost::stacktrace_addr2line)
target_compile_definitions(Errantibus PRIVATE BOOST_STACKTRACE_USE_ADDR2LINE)
target_compile_options(Errantibus PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
target_include_directories(Errantibus PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>)

