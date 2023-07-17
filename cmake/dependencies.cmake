find_package(OpenCV 4.5.4 REQUIRED)
find_package(Threads REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

if (TTOOL_BUILD_TTOOL_EXE)
    find_package(assimp REQUIRED)
    find_package(glog REQUIRED)
    find_package(OpenGL REQUIRED)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glmT)
list(APPEND TTOOL_INSTALL_TARGETS glm)
#set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/glmT)

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
