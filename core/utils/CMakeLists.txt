cmake_minimum_required(VERSION 3.9)
project(csimUtils)

# 优先使用工程内置的 sed（third_party/msys/usr/bin），否则回退到 PATH 查找
set(CSIM_BUNDLED_SED "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/msys/usr/bin/sed.exe")
if(EXISTS "${CSIM_BUNDLED_SED}")
    set(SED_EXECUTABLE "${CSIM_BUNDLED_SED}")
    message(STATUS "Using bundled sed: ${SED_EXECUTABLE}")
else()
    set(SED_EXECUTABLE sed)
endif()

# Generate error code descriptors.
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/errors-generated.h
    COMMAND ${SED_EXECUTABLE} -f ${CMAKE_CURRENT_SOURCE_DIR}/errors.sed "${CMAKE_SOURCE_DIR}/include/csim/utils/errors.h" > ${CMAKE_CURRENT_BINARY_DIR}/errors-generated.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/errors.sed "${CMAKE_SOURCE_DIR}/include/csim/utils/errors.h"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${PROJECT_NAME}
    ${CMAKE_CURRENT_BINARY_DIR}/errors-generated.h
    ${CMAKE_CURRENT_SOURCE_DIR}/errors.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/string.cc
)
