cmake_minimum_required(VERSION 3.5)

project(TTool VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_TSEGMENT_AS_API ON CACHE BOOL "Do not build tsegment exe")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/TSegment)

add_executable(ttool src/main.cc)


# target_include_directories(ttool PUBLIC deps/TSegment)
find_package(OpenCV 4.5.5 REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV 4.5.5 not found")
endif()

find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "Threads not found")
endif()

target_include_directories(ttool PRIVATE 
                           ${CMAKE_CURRENT_SOURCE_DIR}/deps/TSegment
                           ${OpenCV_INCLUDE_DIRS})
target_link_libraries(ttool ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})

# #------------------------------------------------------------------------------------------------------------
# # Check and update git submodules
# #------------------------------------------------------------------------------------------------------------

# Download and update submodules with latest remote version
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update with latest commit")

        execute_process(
            COMMAND ${GIT_EXECUTABLE} --version
            OUTPUT_VARIABLE GIT_VERSION_STRING
        )
        string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" GIT_VERSION_STRING ${GIT_VERSION_STRING})
        set(GIT_VERSION_MAJOR ${CMAKE_MATCH_1})
        set(GIT_VERSION_MINOR ${CMAKE_MATCH_2})
        set(GIT_VERSION_PATCH ${CMAKE_MATCH_3})

        if(NOT (GIT_VERSION_MAJOR GREATER 1 OR (GIT_VERSION_MAJOR EQUAL 1 AND GIT_VERSION_MINOR GREATER 8)))
            message(FATAL_ERROR "Git version 1.8 or greater is required.")
        endif()
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule sync
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive --remote failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()