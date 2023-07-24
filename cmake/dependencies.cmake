find_package(OpenCV 4.5.4 REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV 4.5.4 not found")
endif()

find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "Threads not found")
endif()

#GLFW
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found")
endif()

# libtorch
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/libtorch")
    set(EXTRACT_LIBTORCH_CMD "${PROJECT_SOURCE_DIR}/util/extract_libtorch.sh")
    execute_process(
        COMMAND chmod +x ${EXTRACT_LIBTORCH_CMD}
        COMMAND ${EXTRACT_LIBTORCH_CMD} ${PROJECT_SOURCE_DIR}
    )
endif()

# set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/deps/libtorch")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/libtorch")
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/libtorch)
find_package(Torch REQUIRED)

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

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glmT)
set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/glmT)