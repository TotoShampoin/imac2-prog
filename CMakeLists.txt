cmake_minimum_required(VERSION 3.5)
project(BoidsImac)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/sources)
set(INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/includes)
set(VENDORS_DIRECTORY ${CMAKE_SOURCE_DIR}/vendors)
set(SUBMODULES_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules)
set(TEST_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
set(TESPERIMENT_DIRECTORY ${CMAKE_SOURCE_DIR}/tesperiment)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIRECTORY}/*.cpp)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIRECTORY}/*.hpp)
file(GLOB_RECURSE TEST_FILES ${TEST_DIRECTORY}/*.cpp)

include(${VENDORS_DIRECTORY}/imgui.cmake)
include(${VENDORS_DIRECTORY}/CMakeUtils/files_and_folders.cmake)

add_subdirectory(${SUBMODULES_DIRECTORY}/totogl)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRECTORY})
include_directories(${PROJECT_NAME} PUBLIC ${VENDORS_DIRECTORY})
target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL glfw GLU GLEW glm imgui totogl)

Cool__target_copy_folder(${PROJECT_NAME} assets)

enable_testing()
add_executable(${PROJECT_NAME}_test ${TEST_FILES})
target_link_libraries(${PROJECT_NAME}_test GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)

add_executable(${PROJECT_NAME}_tesperiment ${TESPERIMENT_DIRECTORY}/main.cpp)

