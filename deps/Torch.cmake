# libtorch
# download from the internet (old version)
# download_external_project(libtorch
#     URL "https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.0.1%2Bcpu.zip"
#     PATCH libtorch-optional-c++17.patch
#     THIRD_PARTY_DIR deps)

# download from the internet (new version)
# download_external_project(libtorch
#     URL "https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.5.0%2Bcpu.zip"
#     THIRD_PARTY_DIR deps)

# using local file
download_external_project(libtorch
    URL "file://${PROJECT_SOURCE_DIR}/assets/libtorch-cxx11-abi-shared-with-deps-2.5.0+cpu.zip"
)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/libtorch")
find_package(Torch REQUIRED)

set(TTOOL_TORCH_INCLUDE_DIRS ${TORCH_INCLUDE_DIRS} CACHE INTERNAL "")
set(TTOOL_TORCH_LIBRARIES ${TORCH_LIBRARIES} CACHE INTERNAL "")
