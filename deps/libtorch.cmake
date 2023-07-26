# libtorch
download_external_project(libtorch
    URL "https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.0.1%2Bcpu.zip"
    THIRD_PARTY_DIR deps)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/libtorch")
find_package(Torch REQUIRED)
