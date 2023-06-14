include(${PROJECT_SOURCE_DIR}/cmake/config.cmake.in)
include(${PROJECT_SOURCE_DIR}/cmake/options.cmake)

if(OPT_REFRESH_DATASET)
    if(EXISTS ${PROJECT_SOURCE_DIR}/assets/toolheads)
        message(STATUS "Removing dataset folder")
        file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/assets/toolheads)
    endif()
endif()

if(UNIX AND NOT APPLE)
    # A. download dataset
    set(DOWNLOAD_CMD "${PROJECT_SOURCE_DIR}/util/download_dataset.sh")
    execute_process(
        COMMAND chmod +x ${DOWNLOAD_CMD}
        COMMAND ${DOWNLOAD_CMD} ${__DSET_DOI__} ${PROJECT_SOURCE_DIR}/assets/toolheads RESULT_VARIABLE ret
    )
    # B. load dataset data to config ttool
    if(ret EQUAL 0)
        set(LOADER_CMD "${PROJECT_SOURCE_DIR}/util/load_dataset_2_config.py")
        execute_process(
            COMMAND chmod +x ${LOADER_CMD}
            COMMAND ${LOADER_CMD} -p ${PROJECT_SOURCE_DIR}/assets/toolheads -c ${__TTOOL_CONFIG_PATH__}
            )
    endif()
else()
    message(FATAL_ERROR "This dataset download only works on UNIX")
endif()
