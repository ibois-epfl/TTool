# TTool


![example workflow](https://github.com/ibois-epfl/TTool/actions/workflows/docker-cmake-build.yml/badge.svg)


# Quick Start
Make sure that [docker-desktop is installed](https://www.docker.com/products/docker-desktop/)
```bash
./build_docker.sh
sudo docker run -it ttool /bin/bash
./build/tool
```

# Development Environment Setup (for VSCode)
0. make sure that [docker-desktop is installed](https://www.docker.com/products/docker-desktop/)
1. clone this repository __with submodules__
```
git clone --recurse-submodules -j8 git@github.com:ibois-epfl/TTool.git
```
2. open the project with VSCode
```
code TTool
```
3. install recommended extensions when prompted
4. 

This is the repository hosting the API for TTool. It is a program able to detect the 6dof of a fix toolhead from the feed of a fix camera view.

This is the structure of the API. Each sub-API is imported as a git submodule and integrate it from the main `CMakeList.txt` file

```mermaid
flowchart TB
    A(TTool API)
    B(TSegment API) --> A
    C(TSlet API) --> A
    D(ML API*) --> A

    A --> E(Augmented Carpentry)
```
*ML API is the ML based global pose detection and it needs to *
