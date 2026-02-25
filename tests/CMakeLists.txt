cmake_minimum_required(VERSION 3.24...4.2)
project(libtcod-path-tests CXX)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG 29c9844f688acb27c87338c39cd186ebfe41aa19 # v3.13.0
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS CONFIG
)
FetchContent_MakeAvailable(Catch2)

file(GLOB SRC_FILES CONFIGURE_DEPENDS test_*.cpp)

add_executable(unittest ${SRC_FILES})
target_link_libraries(unittest libtcod-path::libtcod-path Catch2::Catch2 Catch2::Catch2WithMain)
target_compile_features(unittest PUBLIC cxx_std_20)
target_compile_definitions(unittest PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)

if(MSVC)
  target_compile_options(unittest PRIVATE /W4)
  target_compile_options(unittest PRIVATE /utf-8)
else()
  target_compile_options(unittest PRIVATE -Wall -Wextra)
endif()

include(CTest)
add_test(
    NAME unittest
    COMMAND unittest
)
