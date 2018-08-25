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
    main

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
)

set_target_properties(
    main
    PROPERTIES

    CXX_STANDARD 11
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
)

set(COMMON_OPTIONS "-Wall -Wextra -Wno-reorder")


target_compile_options(
    main
    PRIVATE
    #-g -Wall -Wextra -Wno-reorder
    -O3 -fopenmp -Wall -Wextra -Wno-reorder
)

#target_compile_options(
#    main
#    PRIVATE
#    "$<$<CONFIG:RELASE>: ${COMMON_OPTIONS} -O3 -fopenmp>"
#)


target_link_libraries(
    main
    PRIVATE
    -L/usr/lib/nvidia-396 -lOpenGL -lEGL
)
