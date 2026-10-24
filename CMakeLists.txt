cmake_minimum_required(VERSION 3.9)
project(fastcsim)

# 模型加载方式：OFF=动态加载 DLL(默认)，ON=静态链接进可执行文件
option(CSIM_USE_STATIC_MODELS "Build component models statically (no model DLLs)" OFF)
if(CSIM_USE_STATIC_MODELS)
    add_definitions(-DCSIM_USE_STATIC_MODELS)
endif()

# 是否构建 GoogleTest 单元测试（testcases/）
option(CSIM_BUILD_TESTS "Build GoogleTest unit tests (testcases/)" OFF)

if(MINGW)
    # 以下为 MinGW/GCC 专属编译与链接选项；MSVC 无对应参数
    set(COMMON_WARNINGS					"-Wall -Wno-unused-function -Wno-unused-variable")
    set(CMAKE_CXX_FLAGS           		"${COMMON_WARNINGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
elseif(UNIX)
    set(COMMON_WARNINGS					"-Wall -Wno-unused-function -Wno-unused-variable")
    set(CMAKE_CXX_FLAGS           		"${COMMON_WARNINGS}")
endif()
if(MINGW OR UNIX)
    set(CMAKE_MODULE_LINKER_FLAGS_PROFILE "-pg")
    set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "-pg")
endif()
if(MSVC)
    # 源码含 UTF-8 中文注释；MSVC 默认按系统代码页(GBK)读取会破坏注释/字符串。
    # /utf-8 让 cl.exe 按 UTF-8 解析源文件，同时避免 C4819 告警。
    # 项目使用 C++17（结构化绑定等），MSVC 默认仅 C++14，需显式指定。
    add_compile_options(/utf-8 /std:c++17)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
if(UNIX)
    # 动态模式：circuit.exe 依赖 libcsim.so，且 libcsim.so 通过 dlopen 按名加载模型 .so。
    # dlopen 不搜可执行文件目录，只按 ld.so 规则搜索（含调用者的 RPATH/RUNPATH）。
    # 给所有目标加构建 RPATH 指向 lib/，使动态模式开箱即用，无需手动设置 LD_LIBRARY_PATH。
    set(CMAKE_BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
endif()
#
# Check platform
#
if (WIN32)
	add_definitions(-DCSIM_IN_WIN32)
elseif (UNIX)
	add_definitions(-DCSIM_IN_UNIX)
else ()
	message(FATAL_ERROR "Unsupported platform ${CMAKE_SYSTEM}")
endif ()
if (CYGWIN)
    add_definitions(-DCSIM_IN_CYGWIN)
endif ()

#
# 工程内置的 bison/flex（third_party/msys/usr/bin），避免依赖外部安装
# 在 find_package 之前设置 BISON_EXECUTABLE / FLEX_EXECUTABLE 即优先使用内置工具。
#
set(CSIM_BUNDLED_BIN "${CMAKE_CURRENT_SOURCE_DIR}/third_party/msys/usr/bin")
if(EXISTS "${CSIM_BUNDLED_BIN}/bison.exe" AND NOT BISON_EXECUTABLE)
    set(BISON_EXECUTABLE "${CSIM_BUNDLED_BIN}/bison.exe" CACHE FILEPATH "Bison executable (bundled in third_party/msys)")
    message(STATUS "Using bundled bison: ${BISON_EXECUTABLE}")
endif()
if(EXISTS "${CSIM_BUNDLED_BIN}/flex.exe" AND NOT FLEX_EXECUTABLE)
    set(FLEX_EXECUTABLE "${CSIM_BUNDLED_BIN}/flex.exe" CACHE FILEPATH "Flex executable (bundled in third_party/msys)")
    message(STATUS "Using bundled flex: ${FLEX_EXECUTABLE}")
endif()

#
# Find bison
#
find_package(BISON 2.4 REQUIRED)
if(BISON_FOUND)
else()
message(FATAL_ERROR "BISON is required but not found")
endif()

#
# Find Flex
#
find_package(FLEX 2.5.9  REQUIRED)
if(FLEX_FOUND)
else()
  message(FATAL_ERROR "BISON is required but not found" )
endif()


add_subdirectory("${PROJECT_SOURCE_DIR}/core/")
if(CSIM_BUILD_TESTS)
    add_subdirectory("${PROJECT_SOURCE_DIR}/testcases")
endif()
add_executable(circuit main.cc)
if(CSIM_USE_STATIC_MODELS)
    # 静态模式：模型源文件直接编入可执行文件（不生成模型 DLL）
    target_sources(circuit PRIVATE
        "${PROJECT_SOURCE_DIR}/core/models/Resistor.cc"
        "${PROJECT_SOURCE_DIR}/core/models/VDC.cc"
        "${PROJECT_SOURCE_DIR}/core/models/VAC.cc"
        "${PROJECT_SOURCE_DIR}/core/models/IDC.cc"
        "${PROJECT_SOURCE_DIR}/core/models/IAC.cc"
        "${PROJECT_SOURCE_DIR}/core/models/VCVS.cc"
        "${PROJECT_SOURCE_DIR}/core/models/CCVS.cc"
        "${PROJECT_SOURCE_DIR}/core/models/VCCS.cc"
        "${PROJECT_SOURCE_DIR}/core/models/CCCS.cc"
        "${PROJECT_SOURCE_DIR}/core/models/PN.cc"
        "${PROJECT_SOURCE_DIR}/core/models/Capacitor.cc"
        "${PROJECT_SOURCE_DIR}/core/models/Inductor.cc"
        "${PROJECT_SOURCE_DIR}/core/models/bsim3v32/bsim3v32.cc"
        "${PROJECT_SOURCE_DIR}/core/models/bsim3v32/bsim3v32-status.cc"
        "${PROJECT_SOURCE_DIR}/core/models/bsim3v32/bsim3v32-stamp.cc"
        "${PROJECT_SOURCE_DIR}/core/models/bsim3v32/bsim3v32-model.cc"
        "${PROJECT_SOURCE_DIR}/core/models/bsim3v32/bsim3v32-conv.cc"
        "${PROJECT_SOURCE_DIR}/core/models/static_registry.cc"
    )
endif()
set_target_properties(circuit PROPERTIES OUTPUT_NAME circuit)
set_target_properties(circuit PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin) # 指定可执行文件的输出目录  
target_link_libraries(circuit csim)
if(MINGW OR UNIX)
    target_link_libraries(circuit pthread)
endif()
target_include_directories(circuit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(circuit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/csim/internal)

# 定义要复制的 MinGW 运行时 DLL 列表（仅 MinGW 需要；MSVC 无这些 DLL）
if(MINGW)
    set(EXTERNAL_DLLS
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libgcc_s_seh-1.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libstdc++-6.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libwinpthread-1.dll"
    )

    # 复制到 bin 目录
    foreach(dll ${EXTERNAL_DLLS})
        add_custom_command(TARGET circuit POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${dll}
                $<TARGET_FILE_DIR:circuit>
        )
    endforeach()
endif()