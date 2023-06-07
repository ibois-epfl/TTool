include(${PROJECT_SOURCE_DIR}/cmake/config.cmake.in)

if(UNIX AND NOT APPLE)
    set(DOWNLOAD_CMD "${PROJECT_SOURCE_DIR}/util/download_dataset.sh")
    execute_process(
        COMMAND chmod +x ${DOWNLOAD_CMD}
        COMMAND ${DOWNLOAD_CMD} ${DSET_DOI} ${PROJECT_SOURCE_DIR}/assets/toolheads
    )
else()
    message(FATAL_ERROR "This dataset download only works on UNIX")
endif()