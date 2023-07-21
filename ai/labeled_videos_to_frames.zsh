DATADIR="/data/ENAC/iBOIS/labeled_fabrication_videos"
IMAGEDIR="/data/ENAC/iBOIS/labeled_fabrication_images"

DELIMITER=" "
LIST_OF_VIDS="03${DELIMITER}04${DELIMITER}18"

for VIDDIR in $DATADIR/*
do
    VIDNAME="$(basename ${VIDDIR})"
    if [[ "${LIST_OF_VIDS}" =~ "(${DELIMITER}|^)${VIDNAME}(${DELIMITER}|$)" ]]
    then
        VIDFILE="${VIDDIR}/${VIDNAME}_video.mp4"
        echo ${VIDFILE}
        ffmpeg -i $VIDFILE "${IMAGEDIR}/${VIDNAME%.*}_%09d.png"
    fi
done
