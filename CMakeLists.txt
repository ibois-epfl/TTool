cmake_minimum_required(VERSION 3.21)

project(TTool VERSION "0.1.0" LANGUAGES CXX)

# zenodo direct doi link (to change if dataset us updated)
set(TTOOL_DSET_DOI "https://zenodo.org/record/8229361" CACHE INTERNAL "")
include(cmake/dataset.cmake)

file(GLOB glsl_SOURCES src/shader/*.glsl)
source_group("shader" FILES ${glsl_SOURCES})

file(GLOB TTool_SOURCES src/*.cc)
list(REMOVE_ITEM TTool_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc)

file(GLOB TTOOL_PUBLIC_HDRS include/*.hh)

set(TTOOL_INSTALL_TARGETS ttool)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)

add_library(ttool SHARED ${TTool_SOURCES} ${glsl_SOURCES})
message("MESSAGE FROM TTOOL CMAKELISTS")
add_library(TTool::ttool ALIAS ttool)

target_compile_features(ttool PUBLIC cxx_std_17)
message("TORCH LIBRARIES from TTOOL CMAKELISTS ${TORCH_LIBRARIES} ${TTOOL_TORCH_LIBRARIES}")
target_link_libraries(ttool
    PUBLIC
      ${OpenCV_LIBS}
      GLEW::GLEW
      assimp
      glm::glm
      glog::glog
      torch
)

set_property(TARGET ttool PROPERTY PUBLIC_HEADER ${TTOOL_PUBLIC_HDRS})
target_include_directories(ttool
    PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
      INTERFACE $<INSTALL_INTERFACE:include/TTool>
)


message("TORCH VARIABLES=====================")
message(STATUS "TOOL_TORCH_INCLUDE_DIRS: ${TTOOL_TORCH_INCLUDE_DIRS}")

message("TORCH INCLUDE DIRS from TTOOL CMAKELISTS ${TORCH_INCLUDE_DIRS}")

message("CMAKE PREFIX PATH" ${CMAKE_PREFIX_PATH})
# Not sur it is a good idea, it will fail on installed version (NR)
set(__TTOOL_CONFIG_PATH__ "${CMAKE_CURRENT_SOURCE_DIR}/assets/config.yml")
set(__TTOOL_ROOT_PATH__ "${CMAKE_CURRENT_SOURCE_DIR}")

# # create TTool executable
if (TTOOL_BUILD_TTOOL_EXE)
    add_executable(ttoolexec src/main.cc)
    target_link_libraries(ttoolexec
        TTool::ttool
        OpenGL::GL
        OpenGL::GLU
        glfw
        GLUT::GLUT
    )
    list(APPEND TTOOL_INSTALL_TARGETS ttoolexec)

    target_compile_definitions(ttoolexec PRIVATE
      __TTOOL_CONFIG_PATH__="${__TTOOL_CONFIG_PATH__}"
      __TTOOL_ROOT_PATH__="${__TTOOL_ROOT_PATH__}")
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

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )

write_basic_package_version_file(${PROJECT_BINARY_DIR}/TToolConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(
  FILES
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

if(TTool_USE_EXTERNAL_TORCH)
  install(IMPORTED_RUNTIME_ARTIFACTS torch
    RUNTIME_DEPENDENCY_SET torch-set)
  install(RUNTIME_DEPENDENCY_SET torch-set)
  install(DIRECTORY deps/libtorch/include deps/libtorch/share
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    COMPONENT Developpment)
endif()
