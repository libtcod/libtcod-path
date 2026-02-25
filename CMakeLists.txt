cmake_minimum_required(VERSION 3.24...4.2)

option(LIBTCODPATH_CPACK "Enable CPack" OFF)
option(LIBTCODPATH_TESTS "Build and enable unit tests" OFF)
option(LIBTCODPATH_INSTALL "Enable install targets." ON)

set(LIBTCODPATH_VERSION 0.1)

project(
    libtcod-path
    VERSION ${LIBTCODPATH_VERSION}
    LANGUAGES C CXX
)

add_subdirectory(src)

if(LIBTCODPATH_TESTS)
    add_subdirectory(tests)
endif()

if(LIBTCODPATH_CPACK)
    if(WIN32)
        set(CPACK_GENERATOR "ZIP")
    else()
        set(CPACK_GENERATOR "TGZ")
    endif()
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    include(CPack)
endif()
