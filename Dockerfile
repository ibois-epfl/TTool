FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive # ignore user input required

# install required dependencies
RUN apt-get -qq update && apt-get -qq -y install \
  g++ cmake git git-lfs \
  libopencv-dev=4.5.4+dfsg-9ubuntu4 \
  libassimp-dev \
  libgoogle-glog-dev \
  freeglut3 libglew-dev libglfw3-dev libglm-dev \
  unzip \
  && rm -rf /var/lib/apt/lists/*

# For downloading the dataset
RUN apt-get -qq update && apt-get -qq -y install \
  python3 \
  wget curl patch \
  && rm -rf /var/lib/apt/lists/*

#COPY . .
#WORKDIR .

# Clear CMakeCache for Docker (this line is mainly for VSCode dev container)
#RUN find . -name "CMakeCache.txt" -type f -delete

# Config/build cmake 
#RUN ./configure.sh
#RUN ./build.sh
