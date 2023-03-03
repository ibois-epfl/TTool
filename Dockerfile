FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive # ignore user input required

# install required dependencies
RUN apt-get -y update && apt-get install -y
RUN apt-get -y install g++ cmake git
RUN apt-get -y install libopencv-dev=4.5.4+dfsg-9ubuntu4

COPY . .
WORKDIR .

# Config/build cmake 
RUN ./configure.sh
RUN ./build.sh

# Launch built application
CMD ["./build/ttool"]