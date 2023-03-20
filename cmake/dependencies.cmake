find_package(OpenCV 4.5.4 REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV 4.5.4 not found")
endif()

find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "Threads not found")
endif()

if (WITH_CGAL)
    # use local eigen
    find_package(Eigen3 3.3 REQUIRED NO_MODULE)

    find_library(GMP_LIB NAMES "gmp" PATHS ${CMAKE_LIBRARY_PATH})
    include(
        Eigen3::Eigen
        gmp
    )
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/CGAL/include
    )
endif()

if (BUILD_TTOOL_EXE)
    find_package(assimp REQUIRED)
    if(NOT assimp_FOUND)
        message(FATAL_ERROR "assimp not found")
    endif()

    find_package(glog REQUIRED)
    if(NOT glog_FOUND)
        message(FATAL_ERROR "glog not found")
    endif()

    find_package(OpenGL REQUIRED)
    if(NOT OpenGL_FOUND)
        message(FATAL_ERROR "OpenGL not found")
    endif()

    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5OpenGL REQUIRED)

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