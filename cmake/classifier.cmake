
if(UNIX AND NOT APPLE)
    set(LOADER_CMD "${PROJECT_SOURCE_DIR}/util/load_labels_2_config.py")
    execute_process(
            COMMAND chmod +x ${LOADER_CMD}
            COMMAND ${LOADER_CMD} -s ${CMAKE_CURRENT_SOURCE_DIR} -c ${__TTOOL_CONFIG_PATH__}
    )
endif()
