cmake_minimum_required(VERSION 3.5)

project(kdeeb)

#set (CMAKE_CXX_STANDARD 11)

OPTION (USE_OpenMP "Use OpenMP" ON)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

add_executable(
    run

    ${SRC_DIR}/Main.cpp
    ${SRC_DIR}/Application.cpp
    ${SRC_DIR}/Framebuffer.cpp
    ${SRC_DIR}/Image.cpp
    ${SRC_DIR}/Shader.cpp
    ${SRC_DIR}/Application_Input.cpp
    ${SRC_DIR}/Frame.cpp
    ${SRC_DIR}/Iteration.cpp
    ${SRC_DIR}/Point.cpp
    ${SRC_DIR}/Steps.cpp
    ${SRC_DIR}/DataSet.cpp
    ${SRC_DIR}/Gradient.cpp
    ${SRC_DIR}/Line.cpp
    ${SRC_DIR}/RenderException.cpp
    ${SRC_DIR}/Waypoint.cpp
    ${SRC_DIR}/GridGraph.cpp
    ${SRC_DIR}/MinCutSolver.hpp
)

set_target_properties(
    run
    PROPERTIES

    CXX_STANDARD 11
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin"

    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

set(COMMON_OPTIONS "-Wall -Wextra -Wno-reorder")


target_compile_options(
    run
    PRIVATE
    -g -Wall -Wextra -Wno-reorder
    #-O3 -fopenmp -Wall -Wextra -Wno-reorder
)

#target_compile_options(
#    main
#    PRIVATE
#    "$<$<CONFIG:RELASE>: ${COMMON_OPTIONS} -O3 -fopenmp>"
#)


target_link_libraries(
    run
    PRIVATE
    -L/usr/lib/nvidia-410 -lOpenGL -lEGL
    -L/usr/local/lib
    -lthrift
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/mapd/libmapd_sample_thrift.a
    -lpistache
    -lpthread
)

