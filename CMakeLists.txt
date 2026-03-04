cmake_minimum_required(VERSION 3.14)
project(SolarSystem
    VERSION 1.0.0
    DESCRIPTION "A polymorphic data structure manager using Solar System metaphors"
    LANGUAGES CXX
)

# ─────────────────────────────────────────────
#  C++ Standard
# ─────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─────────────────────────────────────────────
#  Include Directories
# ─────────────────────────────────────────────
include_directories(${CMAKE_SOURCE_DIR}/include)

# ─────────────────────────────────────────────
#  Main Executable
# ─────────────────────────────────────────────
add_executable(SolarSystem
    src/main.cpp
    src/SolarSystem.cpp
)

# ─────────────────────────────────────────────
#  Google Test (fetched automatically)
# ─────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# Prevent overriding the parent project's compiler/linker settings (Windows)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ─────────────────────────────────────────────
#  Unit Test Executable
# ─────────────────────────────────────────────
enable_testing()

add_executable(unit_tests
    tests/unit_tests.cpp
    src/SolarSystem.cpp
)

target_link_libraries(unit_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
