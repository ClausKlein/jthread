cmake_minimum_required(VERSION 3.13)

## use ccache if found
find_program(CCACHE_EXECUTABLE "ccache" HINTS /usr/local/bin /opt/local/bin)
if(CCACHE_EXECUTABLE AND NOT CMAKE_TOOLCHAIN_FILE)
    message(STATUS "use ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}" CACHE PATH "ccache" FORCE)
endif()


project(jthread LANGUAGES CXX)

set(CMAKE_MODULE_PATH $ENV{HOME}/cmake)

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-Wpedantic -Wextra -Wall")
    option(USE_LCOV "Use GNU Test Coverage framework" OFF)
    find_library(PTHREAD_LIB pthread REQUIRED)
endif()


enable_testing()


set(cxx_sources
test_cv.cpp
test_cvcb.cpp
test_cvprodcons.cpp
test_cvrace.cpp
test_cvrace_hh.cpp
test_cvrace_pred.cpp
test_cvrace_stop.cpp
test_jthread1.cpp
test_jthread2.cpp
test_stoken.cpp
test_stokencb.cpp
test_stokenrace.cpp
test_stopcb.cpp
)

foreach(source ${cxx_sources})
    string(REGEX REPLACE "\.cpp$" "" program ${source})
    add_executable(${program} ${program}.cpp)
    set_target_properties(${program} PROPERTIES CXX_STANDARD 17)
    target_link_libraries(${program} ${PTHREAD_LIB})
    add_test(NAME ${program} COMMAND ${program})
endforeach()


list(APPEND CMAKE_MODULE_PATH $ENV{HOME}/Workspace/cmake)
if(USE_LCOV)
    set(CMAKE_BUILD_TYPE "Coverage" CACHE STRING "build for lcov" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)

    set(COVERAGE_EXCLUDES '/usr/local/*' '/opt/local/*')
    setup_target_for_coverage(
        NAME lcov            # Name for custom target.
        EXECUTABLE ctest     # Name of the test driver executable that runs the tests.
                        # NOTE! This should always have a ZERO as exit code
                        # otherwise the coverage generation will not complete.
        ##TODO coverage # Name of output directory.
    )
endif() #CMAKE_BUILD_TYPE STREQUAL "Coverage"

