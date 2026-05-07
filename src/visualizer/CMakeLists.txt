include(FetchContent)

FetchContent_Declare(
    libtcod
    GIT_REPOSITORY https://github.com/libtcod/libtcod
    GIT_SUBMODULES
    GIT_TAG 2.2.2-73-gb122e2a4
)
set(LIBTCOD_THREADS OFF)
set(LIBTCOD_ZLIB OFF)

FetchContent_MakeAvailable(libtcod SDL3)

project(
    visualizer
    VERSION ${LIBTCODPATH_VERSION}
    LANGUAGES C CXX
)

file(GLOB ${PROJECT_NAME}_SOURCES CONFIGURE_DEPENDS *.cpp)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE c_std_99 cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE
    libtcod-path::libtcod-path
    libtcod::libtcod
    SDL3::SDL3
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
