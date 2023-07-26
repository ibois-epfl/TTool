cmake_minimum_required(VERSION 3.21)

project(TTool VERSION "0.1.0" LANGUAGES CXX)
include(${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" DESTINATION share/${PROJECT_NAME} )

set(CMAKE_CXX_STANDARD 17)

include(cmake/dataset.cmake)

file(GLOB glsl_SOURCES src/shader/*.glsl)
source_group("shader" FILES ${glsl_SOURCES})

file(GLOB TTool_SOURCES src/*.cc)
list(REMOVE_ITEM TTool_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc)

set(TTOOL_INSTALL_TARGETS ttool_lib)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)

add_library(ttool_lib SHARED
    ${TTool_SOURCES} ${glsl_SOURCES})

add_library(TTool::ttool ALIAS ttool_lib)

target_link_libraries(ttool_lib
    PUBLIC
      ${OpenCV_LIBS}
      glm::glm
      glog::glog
      assimp
      torch
)

set_property(TARGET ttool_lib PROPERTY PUBLIC_HEADER ${TTOOL_PUBLIC_HDRS})
target_include_directories(ttool_lib
    PUBLIC    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    INTERFACE $<INSTALL_INTERFACE:include/TTool>
)

# # create TTool executable
if (TTOOL_BUILD_TTOOL_EXE)
    add_executable(ttoolexec src/main.cc)
    target_link_libraries(ttoolexec
        ttool_lib
        GLEW::GLEW
        OpenGL::GL
        OpenGL::GLU
        glfw
    )
    list(APPEND TTOOL_INSTALL_TARGETS ttoolexec)
endif()

if(NOT TTOOL_TARGETS_EXPORT)
    set(TTOOL_TARGETS_EXPORT ${PROJECT_NAME}Targets)
endif()

include(GNUInstallDirs)
install(TARGETS ${TTOOL_INSTALL_TARGETS}
    EXPORT ${TTOOL_TARGETS_EXPORT}
    LIBRARY
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT Libraries
    ARCHIVE
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT Libraries
    RUNTIME
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT Executables
    PUBLIC_HEADER
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TTool
      COMPONENT Developpment
)

install(EXPORT ${TTOOL_TARGETS_EXPORT}
    NAMESPACE TTool::
    DESTINATION lib/cmake/${PROJECT_NAME}
    COMPONENT Developpment)

# Export for build tree
export(EXPORT ${TTOOL_TARGETS_EXPORT}
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${TTOOL_TARGETS_EXPORT}.cmake")
