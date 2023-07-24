cmake_minimum_required(VERSION 3.21)

project(TTool VERSION "0.1.0" LANGUAGES CXX)
include(${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" DESTINATION share/${PROJECT_NAME} )

set(CMAKE_CXX_STANDARD 17)

include(cmake/dataset.cmake)

FILE(GLOB glsl_SOURCES src/shader/*.glsl)
source_group("shader" FILES ${glsl_SOURCES})

FILE(GLOB TTool_SOURCES src/*.cc)
list(REMOVE_ITEM TTool_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc)
include(cmake/options.cmake)
include(cmake/dependencies.cmake)

add_library(TTool SHARED ${TTool_SOURCES} ${glsl_SOURCES})
target_include_directories(TTool PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIR}
        )
target_link_libraries(TTool PUBLIC
        ${OpenCV_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        glfw
        ${GLFW_LIBRARIES}
        GLEW
        GLU
        glog::glog
        ${OPENGL_LIBRARIES}
        ${ASSIMP_LIBRARIES}
        ${CMAKE_DL_LIBS}
        ${TORCH_LIBRARIES}
        )

# FIXME: attach dependecies to install TTool
install(TARGETS TTool DESTINATION lib)
file(GLOB_RECURSE TTool_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh)
foreach(HEADER_FILE ${TTool_HEADERS})
    get_filename_component(HEADER_FILE_DIR ${HEADER_FILE} DIRECTORY)
    file(RELATIVE_PATH HEADER_FILE_DIR_REL ${CMAKE_CURRENT_SOURCE_DIR}/include ${HEADER_FILE_DIR})
    install(FILES ${HEADER_FILE} DESTINATION include/TTool/${HEADER_FILE_DIR_REL})
endforeach()

# Add uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(TTool_uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# # create TTool executable
if (OPT_BUILD_TTOOL_EXE)
    add_executable(ttoolexec src/main.cc)
    target_link_libraries(ttoolexec
                          TTool
                          )
endif()
