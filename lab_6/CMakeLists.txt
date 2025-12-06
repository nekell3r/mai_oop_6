cmake_minimum_required(VERSION 3.10)
project(lab_6 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories(include)

set(LIBRARY_SOURCES
    src/npc.cpp
    src/bear.cpp
    src/elf.cpp
    src/robber.cpp
    src/npc_factory.cpp
    src/observer.cpp
)

add_library(npc_lib STATIC ${LIBRARY_SOURCES})

add_executable(lab6_main src/main.cpp)
target_link_libraries(lab6_main npc_lib)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(lab6_tests
    tests/test_npc.cpp
    tests/test_factory.cpp
    tests/test_visitor.cpp
)

target_link_libraries(lab6_tests
    npc_lib
    gtest_main
    gtest
)

include(GoogleTest)
gtest_discover_tests(lab6_tests)

install(TARGETS lab6_main DESTINATION bin)



