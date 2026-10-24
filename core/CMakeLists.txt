cmake_minimum_required(VERSION 3.9)
project(csim)
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/../include/csim/internal"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)
if(MSVC)
	# flex 生成的词法分析器 include <unistd.h>；MSVC 无此头文件，注入兼容 shim
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../compat")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/models")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utils")
# Generate error code descriptors.



set(SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/models/base/ModelBase.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/base/Variant.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/base/PropertyBag.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/base/MComplex.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/base/Environment.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/base/compact/SPICE-Compatible.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/base/compact/SPICE-Devsup.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/loader/Loader.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/models/loader/StaticModelLoader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/IntegralHistory.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/LinearSolver.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/LinearSolverGauss.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/LinearSolverLU.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/IntegralCorrector.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/IntegralCorrectorGear.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/IntegralPredictor.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/IntegralPredictorEuler.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver/IntegralPredictorGear.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/optimizer/GminOptimizer.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/Analyzers.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/AnalyzerOP.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/AnalyzerDC.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/AnalyzerAC.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/AnalyzerTran.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/Sweep.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/SweepLinear.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/SweepLogarithm.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/SweepDecade.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/analyzer/SweepOctave.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/parser/ast-hspice.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/parser/algebraic.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Circuit.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Netlist.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dataset.cc"
	${CMAKE_CURRENT_SOURCE_DIR}/parse.cc
	${CMAKE_CURRENT_SOURCE_DIR}/helper.cc
	${CMAKE_CURRENT_SOURCE_DIR}/modelmanager.cc	
)

# Generate lexior
set(FLEX_HSPICE_OUT
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex-hspice.cc )
add_custom_command(
    OUTPUT ${FLEX_HSPICE_OUT}
    COMMAND ${FLEX_EXECUTABLE}
            --outfile=${FLEX_HSPICE_OUT}
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex-hspice.lpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex-hspice.lpp
)
set_source_files_properties(${FLEX_HSPICE_OUT} PROPERTIES GENERATED TRUE)


# Generate parser
set(BISON_HSPICE_OUT
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-hspice.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-hspice.h )
add_custom_command(
    OUTPUT ${BISON_HSPICE_OUT}
    COMMAND ${BISON_EXECUTABLE}
                --defines=${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-hspice.h
                --output=${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-hspice.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-hspice.ypp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-hspice.ypp
)
set_source_files_properties(${BISON_HSPICE_OUT} PROPERTIES GENERATED TRUE)



# Generate lexior
set(FLEX_ALGEBRAIC_OUT
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex-algebraic.cc )
add_custom_command(
    OUTPUT ${FLEX_ALGEBRAIC_OUT}
    COMMAND ${FLEX_EXECUTABLE}
            --outfile=${FLEX_ALGEBRAIC_OUT}
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex-algebraic.lpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex-algebraic.lpp
)
set_source_files_properties(${FLEX_ALGEBRAIC_OUT} PROPERTIES GENERATED TRUE)


# Generate parser
set(BISON_ALGEBRAIC_OUT
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-algebraic.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-algebraic.h )
add_custom_command(
    OUTPUT ${BISON_ALGEBRAIC_OUT}
    COMMAND ${BISON_EXECUTABLE}
                --defines=${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-algebraic.h
                --output=${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-algebraic.cc
                --debug
                ${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-algebraic.ypp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser/parse-algebraic.ypp
)
set_source_files_properties(${BISON_ALGEBRAIC_OUT} PROPERTIES GENERATED TRUE)

#add_library(${PROJECT_NAME} STATIC
if(CSIM_USE_STATIC_MODELS)
    set(CSIM_LINK_TYPE STATIC)
else()
    set(CSIM_LINK_TYPE SHARED)
endif()
add_library(${PROJECT_NAME} ${CSIM_LINK_TYPE}
    ${SRCS}
    ${FLEX_HSPICE_OUT}
    ${BISON_HSPICE_OUT}
    ${FLEX_ALGEBRAIC_OUT}
    ${BISON_ALGEBRAIC_OUT}
)
if(CSIM_USE_STATIC_MODELS)
    # 静态库无需生成导入库
elseif(MINGW)
    target_link_options(${PROJECT_NAME}  PRIVATE -Wl,--out-implib,libmylib.a)
endif()
if(MSVC AND NOT CSIM_USE_STATIC_MODELS)
    # MSVC 动态模式：导出 csim.dll 全部符号供模型 DLL/可执行文件链接；
    # 并按 constants.h 的 DLIB_PREFIX "lib" 设置输出名（MinGW 默认带 lib 前缀）
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
        PREFIX "lib"
    )
endif()
# gcov 为 GCC/Clang 覆盖率运行时；MSVC 无此库
if(MINGW OR UNIX)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
    target_link_libraries(${PROJECT_NAME} csimUtils gcov)
else()
    target_link_libraries(${PROJECT_NAME} csimUtils)
endif()
# Linux：Loader.cc 使用 dlopen/dlsym/dlclose。老版本 glibc(<2.34) 位于 libdl，需显式链接；
# 新版本已并入 libc，-ldl 是无副作用的安全写法。
if(UNIX)
    target_link_libraries(${PROJECT_NAME} dl)
endif()

