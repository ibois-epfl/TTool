cmake_minimum_required(VERSION 3.12)

# include(CPack)

project(ttool VERSION "0.1.0" LANGUAGES CXX)
include(${PROJECT_SOURCE_DIR}/cmake/ttoolConfig.cmake.in)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ttoolConfig.cmake.in ttoolConfig.cmake @ONLY)

set(CMAKE_CXX_STANDARD 17)

include(cmake/dataset.cmake)

# create ttool library dynamically
FILE(GLOB glsl_SOURCES src/shader/*.glsl)
source_group("shader" FILES ${glsl_SOURCES})

FILE(GLOB ttool_SOURCES src/*.cc)
list(REMOVE_ITEM ttool_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc)
include(cmake/options.cmake)
include(cmake/dependencies.cmake)

add_library(ttool SHARED ${ttool_SOURCES} ${glsl_SOURCES})
target_link_libraries(ttool PUBLIC
        ${OpenCV_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        Qt5::Widgets
        Qt5::OpenGL
        glog::glog
        ${OPENGL_LIBRARIES}
        ${ASSIMP_LIBRARIES})
target_include_directories(ttool PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${OpenCV_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR})

install(TARGETS ttool DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/ttool.hh DESTINATION include)




# export the package to be used by other projects
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(TARGETS ttool FILE ttoolConfig.cmake)

export(PACKAGE ttool)






# TODO: not working for import outside


# Add uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


# find_package(ttool REQUIRED)
# message(STATUS "ttool found: ${ttool_FOUND}")
# message(STATUS "ttool include dir: ${ttool_INCLUDE_DIRS}")
# message(STATUS "ttool library dir: ${ttool_LIBRARY_DIRS}")



# create ttool executable
if (OPT_BUILD_TTOOL_EXE)
    add_executable(ttoolexec src/main.cc)
    target_link_libraries(ttoolexec ttool)
endif()