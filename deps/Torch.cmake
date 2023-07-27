# libtorch
# download_external_project(libtorch
#     URL "https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.0.1%2Bcpu.zip"
#     PATCH libtorch-optional-c++17.patch
#     THIRD_PARTY_DIR deps)

download_external_project(libtorch
    URL "file://${PROJECT_SOURCE_DIR}/assets/libtorch-cxx11-abi-shared-with-deps-2.0.1+cpu.zip"
)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/libtorch")
find_package(Torch REQUIRED)

