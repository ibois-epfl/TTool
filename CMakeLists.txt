cmake_minimum_required(VERSION 3.5)

project(ttool VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_TSEGMENT_AS_API ON CACHE BOOL "Do not build tsegment exe")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/TSegment)

FILE(GLOB glsl_SOURCES src/shader/*.glsl)
source_group("shader" FILES ${glsl_SOURCES})

# target_include_directories(ttool PUBLIC deps/TSegment)
include(cmake/options.cmake)
include(cmake/dependencies.cmake)

if (BUILD_TTOOL_EXE)
    add_executable(ttool
                src/main.cc
                src/camera.cc
                src/viewer.cc
                src/view.cc
                src/transformations.cc
                src/model.cc
                src/global_param.cc
                src/visualizer.hh
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