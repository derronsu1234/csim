cmake_minimum_required(VERSION 3.9)
project(csimModels)

if(CSIM_USE_STATIC_MODELS)
    # 静态模式：模型源文件由顶层 circuit 目标统一编译，此处不构建 DLL
    message(STATUS "Static model mode: model sources are compiled into the executable")
else()
    # 动态模式：每个模型编译为独立 DLL，由 ModelLoader 运行时加载
    add_library(
        csimModelResistor SHARED
        "${CMAKE_CURRENT_SOURCE_DIR}/Resistor.cc"
    )
add_library(
    csimModelVDC SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/VDC.cc"
)
add_library(
    csimModelVAC SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/VAC.cc"
)
add_library(
    csimModelIDC SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/IDC.cc"
)
add_library(
    csimModelIAC SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/IAC.cc"
)
add_library(
    csimModelVCVS SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/VCVS.cc"
)
add_library(
    csimModelCCVS SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/CCVS.cc"
)
add_library(
    csimModelVCCS SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/VCCS.cc"
)
add_library(
    csimModelCCCS SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/CCCS.cc"
)
add_library(
    csimModelPN SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/PN.cc"
)
add_library(
    csimModelCapacitor SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/Capacitor.cc"
)
add_library(
    csimModelInductor SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/Inductor.cc"
)
add_library(
    csimModelBSIM3V32 SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/bsim3v32/bsim3v32.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/bsim3v32/bsim3v32-status.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/bsim3v32/bsim3v32-stamp.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/bsim3v32/bsim3v32-model.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/bsim3v32/bsim3v32-conv.cc"
)

macro(add_csim_model target_name)
    target_link_libraries(${target_name} csim)
    if(MSVC)
        # 与 constants.h 的 DLIB_PREFIX "lib" 保持一致，否则 LoadLibraryA 找不到模型库
        set_target_properties(${target_name} PROPERTIES PREFIX "lib")
    endif()
endmacro()

add_csim_model(csimModelResistor)
add_csim_model(csimModelVDC)
add_csim_model(csimModelVAC)
add_csim_model(csimModelIDC)
add_csim_model(csimModelIAC)
add_csim_model(csimModelVCVS)
add_csim_model(csimModelCCVS)
add_csim_model(csimModelVCCS)
add_csim_model(csimModelCCCS)
add_csim_model(csimModelPN)
add_csim_model(csimModelCapacitor)
add_csim_model(csimModelInductor)
add_csim_model(csimModelBSIM3V32)
endif()
