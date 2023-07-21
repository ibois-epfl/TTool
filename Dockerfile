FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive # ignore user input required

# install required dependencies
RUN apt-get -y update && apt-get install -y
RUN apt-get -y install g++ cmake git
RUN apt-get -y install libopencv-dev=4.5.4+dfsg-9ubuntu4
RUN apt-get -y install libassimp-dev
RUN apt-get -y install libgoogle-glog-dev
RUN apt-get -y install freeglut3-dev
RUN apt-get -y install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools

COPY . .
WORKDIR .

RUN git lfs pull -I assets/libtorch-cxx11-abi-shared-with-deps-2.0.1+cpu.zip

# Clear CMakeCache for Docker (this line is mainly for VSCode dev container)
RUN find . -name "CMakeCache.txt" -type f -delete

# Config/build cmake 
RUN ./configure.sh
RUN ./build.sh