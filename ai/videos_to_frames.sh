DATADIR="/home/aymanns/projects/ENAC_augmented_carpentry/data/raw_data"
IMAGEDIR="/home/aymanns/projects/ENAC_augmented_carpentry/data/images"

for TOOLDIR in $DATADIR/*
do
    TOOLNAME="$(basename ${TOOLDIR})"
    echo ${TOOLNAME}
    for VIDFILE in $TOOLDIR/*
    do
        echo "\t${VIDFILE}"
        VIDNAME="$(basename ${VIDFILE})"
        # echo "\t\t${VIDNAME%.*}"
        ffmpeg -i $VIDFILE -r 1/1 "${IMAGEDIR}/${TOOLNAME}__${VIDNAME%.*}_%09d.png"
    done
done
