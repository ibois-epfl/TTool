xhost +
docker run -it --rm -e DISPLAY=$DISPLAY \
   --privileged \
   -v /tmp/.X11-unix:/tmp/.X11-unix \
   --device=/dev/video0 \
   --env="QT_X11_NO_MITSHM=1" \
   --network=host --user=$(id -u $USER) ttool /bin/bash