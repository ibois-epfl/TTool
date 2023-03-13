cmake_minimum_required(VERSION 3.5)

project(ttool VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_TSEGMENT_AS_API ON CACHE BOOL "Do not build tsegment exe")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/TSegment)

add_executable(ttool src/main.cc src/camera.cc)


# target_include_directories(ttool PUBLIC deps/TSegment)
find_package(OpenCV 4.5.4 REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV 4.5.4 not found")
endif()

find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "Threads not found")
endif()

# use local eigen
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

find_library(GMP_LIB NAMES "gmp" PATHS ${CMAKE_LIBRARY_PATH} )

target_include_directories(ttool PRIVATE 
                           ${CMAKE_CURRENT_SOURCE_DIR}/deps/TSegment
                           ${CMAKE_CURRENT_SOURCE_DIR}/deps/CGAL/include
                           ${OpenCV_INCLUDE_DIRS})
target_link_libraries(ttool ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} Eigen3::Eigen gmp)