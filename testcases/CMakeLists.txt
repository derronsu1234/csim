cmake_minimum_required(VERSION 3.9)
project(csim-tests)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/googletest/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/googletest"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/csim/internal"
)

# add_coverage 可能未定义（顶层未启用覆盖率时），提供空实现兜底
if(NOT COMMAND add_coverage)
    function(add_coverage target)
    endfunction()
endif()

set(TEST_SRCS
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstLoader.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstLinearSolver.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstLinearSolverGauss.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstLinearSolverLU.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/tstOPLinearCircuit.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstOPNonlinearCircuit.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstDCLinearCircuit.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstCapacitorOP.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstInductorOP.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstVACInOP.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstACLinearCircuit.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstIntegralCorrector.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstIntegralPredictor.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstTransientAnalysis.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstOPSources.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstTransientNonlinearCircuit.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstGearTruncationError.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstHSPICEParser.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstAlgebraic.cc"
    #"${CMAKE_CURRENT_SOURCE_DIR}/tstBSIM3v32.cc"
)

add_executable(tstall
    "${CMAKE_CURRENT_SOURCE_DIR}/googletest/src/gtest-all.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/googletest/src/gtest_main.cc"
    ${TEST_SRCS}
)
if(CSIM_USE_STATIC_MODELS)
    # 静态模式：模型源文件直接编入测试可执行文件（与 circuit 一致）
    target_sources(tstall PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/Resistor.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/VDC.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/VAC.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/IDC.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/IAC.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/VCVS.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/CCVS.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/VCCS.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/CCCS.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/PN.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/Capacitor.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/Inductor.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/bsim3v32/bsim3v32.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/bsim3v32/bsim3v32-status.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/bsim3v32/bsim3v32-stamp.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/bsim3v32/bsim3v32-model.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/bsim3v32/bsim3v32-conv.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/../core/models/static_registry.cc"
    )
endif()
target_link_libraries(tstall csim)
if(MINGW OR UNIX)
    target_link_libraries(tstall pthread)
endif()
add_coverage(tstall)

if (enable_coverage)
    add_custom_target(clear_cov
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -DPROJECT_BINARY_DIR="${PROJECT_BINARY_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/clear_coverage.cmake"
        COMMENT "Clearing code coverage reports...")
    
    add_custom_target(clean_gcno
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -DPROJECT_BINARY_DIR="${PROJECT_BINARY_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/clear_coverage_no.cmake"
        COMMENT "Clearing gcno...")
else ()
    add_custom_target(clear_cov)
endif ()

add_custom_target(check COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/tstall
                        COMMENT "Running tests..."
                        DEPENDS clear_cov)

if (enable_coverage)
    find_program(CURL_EXECUTABLE curl)

    if (NOT CURL_EXECUTABLE)
        message(FATAL_ERROR "Coveralls: curl not found! Aborting")
    endif()

    set(COVERALLS_FILE ${CMAKE_BINARY_DIR}/coveralls.json)

    add_custom_target(gen_cov_report
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND gcovr -r .. -e '../testcases/googletest/' --html-details -o coverage.html
        COMMENT "Generating code coverage reports..."
        DEPENDS check)
    
    add_custom_target(gen_coveralls
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND gcovr -r .. -e '../testcases/googletest/' --coveralls -o ${COVERALLS_FILE}
        COMMENT "Generating code coverage for coveralls..."
        DEPENDS check)
    
    add_custom_target(coveralls_upload
        # Upload the JSON to coveralls.
        COMMAND ${CURL_EXECUTABLE}
                -S -F json_file=@${COVERALLS_FILE}
                https://coveralls.io/api/v1/jobs

        DEPENDS gen_coveralls

        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Uploading coveralls output...")

    add_custom_target(coverage DEPENDS gen_cov_report)

    add_custom_target(coveralls DEPENDS coveralls_upload)
endif ()


