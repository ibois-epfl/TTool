FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive # ignore user input required

# install required dependencies
RUN apt-get -y update && apt-get install -y
RUN apt-get -y install g++ cmake git
RUN apt-get -y install libopencv-dev=4.5.4+dfsg-9ubuntu4
RUN apt-get -y install libassimp-dev
RUN apt-get -y install libgoogle-glog-dev
RUN apt-get -y install freeglut3-dev libglew-dev libglfw3 libglfw3-dev

# For extracting the libtorch zip
RUN apt-get -y install unzip

# For downloading the dataset
RUN apt-get install -y python3
RUN  apt-get update \
  && apt-get install -y wget \
  && rm -rf /var/lib/apt/lists/*

COPY . .
WORKDIR .


# Clear CMakeCache for Docker (this line is mainly for VSCode dev container)
RUN find . -name "CMakeCache.txt" -type f -delete

# Config/build cmake 
RUN ./configure.sh
RUN ./build.sh