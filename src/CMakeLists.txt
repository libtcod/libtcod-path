cmake_minimum_required(VERSION 3.24...4.2)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE c_std_99 cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    PREFIX "" # Avoid duplicate lib prefix
    VISIBILITY_INLINES_HIDDEN OFF
    VERIFY_INTERFACE_HEADER_SETS TRUE
    VERSION ${PROJECT_VERSION}
)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

#file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES CONFIGURE_DEPENDS
#    libtcod-path/*.c libtcod-path/*.cpp
#)
#target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_SOURCES})

file(GLOB_RECURSE ${PROJECT_NAME}_INCLUDES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/*.hpp
)
target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET libtcod_path_header_set
    TYPE HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/include
    FILES ${${PROJECT_NAME}_INCLUDES}
)

#if(MSVC)
#  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
#  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
#else()
#  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
#endif()

if(LIBTCODPATH_INSTALL)
    include(GNUInstallDirs)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        FILE_SET libtcod_path_header_set
        RUNTIME LIBRARY ARCHIVE
        COMPONENT Library
    )
    include(CMakePackageConfigHelpers)
    install(
        EXPORT ${PROJECT_NAME}Targets
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        NAMESPACE ${PROJECT_NAME}::
    )
    configure_package_config_file(${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR
    )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
    )
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    )
    install(
        FILES "../LICENSE.txt"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        RENAME copyright
    )
endif()
