DATADIR="/data/ENAC/iBOIS/undistorted_videos"
IMAGEDIR="/data/ENAC/iBOIS/images"

for TOOLDIR in $DATADIR/*
do
    TOOLNAME="$(basename ${TOOLDIR})"
    echo ${TOOLNAME}
    for VIDFILE in $TOOLDIR/*
    do
        echo "\t${VIDFILE}"
        VIDNAME="$(basename ${VIDFILE})"
        # echo "\t\t${VIDNAME%.*}"
        ffmpeg -i $VIDFILE "${IMAGEDIR}/${TOOLNAME}__${VIDNAME%.*}_%09d.png"
    done
done
