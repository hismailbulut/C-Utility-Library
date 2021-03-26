cmake_minimum_required(VERSION 3.19)
project(c_utils)

set(C_UTILS_LIBRARY_SOURCE_FILES
    "src/containers/Array.c"
    "src/containers/UniqueArray.c"
    "src/containers/List.c"
    "src/containers/LinkedList.c"
    "src/containers/Map.c"
    "src/containers/Dictionary.c"
    "src/StringUtils.c"
    "src/MemoryUtils.c"
    "src/FileUtils.c"
    "src/Json.c"
    "src/Timer.c")

add_library(${PROJECT_NAME} ${C_UTILS_LIBRARY_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "include")

if(TESTS_ENABLED)
  add_executable(c_utils_test "test/main.c" "test/tests.c")
  target_include_directories(c_utils_test PUBLIC "include")
  target_include_directories(c_utils_test PUBLIC "test")
  target_link_libraries(c_utils_test PUBLIC ${PROJECT_NAME})
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_compile_definitions(_DEBUG)
endif()
