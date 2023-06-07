include(${PROJECT_SOURCE_DIR}/cmake/config.cmake.in)

# download dataset
if(UNIX AND NOT APPLE)
    set(DOWNLOAD_CMD "${PROJECT_SOURCE_DIR}/util/download_dataset.sh")
    execute_process(
        COMMAND chmod +x ${DOWNLOAD_CMD}
        COMMAND ${DOWNLOAD_CMD} ${__DSET_DOI__} ${PROJECT_SOURCE_DIR}/assets/toolheads
    )
else()
    message(FATAL_ERROR "This dataset download only works on UNIX")
endif()

# load the .obj files to the config.yml of ttool
set(OBJ_FILES "")
# for each folder inside the toolheads folder
file(GLOB TOOLHEADS "${PROJECT_SOURCE_DIR}/assets/toolheads/*")
foreach(TOOLHEAD ${TOOLHEADS})
    # for each .obj file inside the folder
    file(GLOB OBJ_FILES_IN_TOOLHEAD "${TOOLHEAD}/*.obj")
    foreach(OBJ_FILE ${OBJ_FILES_IN_TOOLHEAD})
        # add the .obj file to the list of .obj files
        list(APPEND OBJ_FILES ${OBJ_FILE})
    endforeach()
endforeach()