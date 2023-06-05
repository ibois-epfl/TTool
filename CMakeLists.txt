cmake_minimum_required(VERSION 3.5)

project(ttool VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_TSEGMENT_AS_API ON CACHE BOOL "Do not build tsegment exe")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/TSegment)

set(MODEL_NAMES 
    "ck30"
    "drillBit_14_105"
    "drillBit_20_110"
    "flachbohrer_25"
    "forstnerbohrer_40"
    "holzkombihorer_20"
    "holzspiralbohrer_sp_20"
    "selfFeedBits_40_90"
    "selfFeedBits_50"
)

foreach(MODEL_NAME IN LISTS MODEL_NAMES)
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/assets/toolheads/${MODEL_NAME}.zip")
        message(STATUS "Downloading tool head files from https://zenodo.org/record/8005648/files/${MODEL_NAME}.zip")
        file(DOWNLOAD "https://zenodo.org/record/8005648/files/${MODEL_NAME}.zip" "${PROJECT_SOURCE_DIR}/assets/toolheads/${MODEL_NAME}.zip")
        file(ARCHIVE_EXTRACT INPUT "${PROJECT_SOURCE_DIR}/assets/toolheads/${MODEL_NAME}.zip" DESTINATION "${PROJECT_SOURCE_DIR}/assets/toolheads/")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rm ${PROJECT_SOURCE_DIR}/assets/toolheads/${MODEL_NAME}.zip
        )
    endif()
endforeach()



FILE(GLOB glsl_SOURCES src/shader/*.glsl)
source_group("shader" FILES ${glsl_SOURCES})

FILE(GLOB ttool_SOURCES src/*.cc src/*.hh)
# target_include_directories(ttool PUBLIC deps/TSegment)
include(cmake/options.cmake)
include(cmake/dependencies.cmake)

if (BUILD_TTOOL_EXE)
    add_executable(ttool
                ${ttool_SOURCES}
                ${glsl_SOURCES})
    target_include_directories(ttool PRIVATE 
                            ${CMAKE_CURRENT_SOURCE_DIR}/deps/TSegment
                            ${OpenCV_INCLUDE_DIRS}
                            ${OPENGL_INCLUDE_DIR})
    target_link_libraries(ttool
                        ${OpenCV_LIBS}
                        ${CMAKE_THREAD_LIBS_INIT}
                        Qt5::Widgets
                        Qt5::OpenGL
                        glog::glog 
                        ${OPENGL_LIBRARIES}
                        ${ASSIMP_LIBRARIES})
endif()