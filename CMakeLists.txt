cmake_minimum_required(VERSION 3.16)
project(ecs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

add_library(ecs_core STATIC ./ECSComponentSetup.cpp)
target_include_directories(ecs_core PUBLIC ./inc ./)

# Create your game executable target as usual
add_executable(${PROJECT_NAME} main.cpp 

)

target_link_libraries(${PROJECT_NAME} PRIVATE ecs_core)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)

    add_executable(ecs_tests
        tests/EntityManagerTests.cpp
        tests/ComponentStorageTests.cpp
        tests/EcsRegistryTests.cpp
    )

    target_link_libraries(ecs_tests PRIVATE ecs_core GTest::gtest_main)
    add_test(NAME ecs_tests COMMAND ecs_tests)
endif()


