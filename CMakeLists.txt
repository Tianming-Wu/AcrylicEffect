cmake_minimum_required(VERSION 3.18)

project(AcrylicEffect
    LANGUAGES CXX
    VERSION 1.0.0
)


add_library(AcrylicEffect SHARED src/acrylicEffect.cpp)

target_link_libraries(AcrylicEffect PRIVATE dwmapi)
target_include_directories(AcrylicEffect PUBLIC "include")

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
