cmake_minimum_required(VERSION 3.18)

project(WindowsVisualEffect
    LANGUAGES CXX
    VERSION 7.9.1
    DESCRIPTION "Windows visual effects library for acrylic, mica and blur effects"
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(WINDOWS_VISUAL_EFFECT_BUILD_TESTS "Build tests" OFF)

# Library target
add_library(WindowsVisualEffect SHARED
    include/visualEffect.h 
    include/visualEffect_private.h
    src/visualEffect.cpp
)

# Add debug postfix
set_target_properties(WindowsVisualEffect PROPERTIES
    DEBUG_POSTFIX "d"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Include directories
target_include_directories(WindowsVisualEffect PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Link libraries
target_link_libraries(WindowsVisualEffect PRIVATE dwmapi)

# Require C++17
target_compile_features(WindowsVisualEffect PUBLIC cxx_std_17)

# Tests
if(WINDOWS_VISUAL_EFFECT_BUILD_TESTS)
    add_executable(visual_effect_test test.cpp)
    target_link_libraries(visual_effect_test PRIVATE WindowsVisualEffect)
    set_target_properties(visual_effect_test PROPERTIES
        DEBUG_POSTFIX "d"
    )
endif()

# Install library
install(
    TARGETS WindowsVisualEffect
    EXPORT WindowsVisualEffectTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install headers
install(
    FILES 
        include/visualEffect.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/WindowsVisualEffect
)

# Install export targets
install(
    EXPORT WindowsVisualEffectTargets
    FILE WindowsVisualEffectTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/WindowsVisualEffect
)

# Generate config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/WindowsVisualEffectConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/WindowsVisualEffectConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/WindowsVisualEffect
)

# Generate version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/WindowsVisualEffectConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/WindowsVisualEffectConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/WindowsVisualEffectConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/WindowsVisualEffect
)