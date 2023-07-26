set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
include(external_tools)

find_package(OpenCV 4.5.4 REQUIRED)
find_package(glog REQUIRED)
find_package(GLEW REQUIRED)

if (TTOOL_BUILD_TTOOL_EXE)
    find_package(assimp REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 3.3 REQUIRED)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glmT)
list(APPEND TTOOL_INSTALL_TARGETS glm)

add_external_package(Torch THIRD_PARTY_DIR deps)
