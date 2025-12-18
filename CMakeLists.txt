cmake_minimum_required(VERSION 3.18)

project(AcrylicEffect
    LANGUAGES CXX
    VERSION 1.0.0
)

option(ARCYLIC_BUILD_TESTS "Build tests" OFF)

add_library(AcrylicEffect SHARED
    include/acrylicEffect.h src/acrylicEffect.cpp
)

target_include_directories(AcrylicEffect PUBLIC "include/")
target_link_libraries(AcrylicEffect PRIVATE dwmapi)

if(ARCYLIC_BUILD_TESTS)
    add_executable(acrylic_test test.cpp)
    target_link_libraries(acrylic_test PRIVATE AcrylicEffect)
endif()


install(
    TARGETS AcrylicEffect
    LIBRARY DESTINATION ${INSTALL_LIBDIR}
    INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
)

target_include_directories(AcrylicEffect PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(FILES "include/acrylicEffect.h"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
)