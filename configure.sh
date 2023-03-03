
#!/bin/bash


# Check if build/ dir is present
if [ -d build ]; then
    echo "[INFO] build/ dir already present."
else
    mkdir build
fi

# update the submodules
git submodule update --init --recursive

# execute cmake config
while getopts ":c" opt; do
    case $opt in
        c)  # purge build/ dir
            echo "[INFO] Cleaning build/ dir before configure..."
            if [ -d build ]; then
                rm -rf build/*
            fi
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
    esac
done


cmake -S . -B build