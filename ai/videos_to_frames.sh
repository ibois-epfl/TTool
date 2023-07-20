DATADIR="/data/ENAC/iBOIS/test_dataset"
IMAGEDIR="/data/ENAC/iBOIS/test_dataset/images"

for TOOLDIR in $DATADIR/*
do
    TOOLNAME="$(basename ${TOOLDIR})"
    echo ${TOOLNAME}
    if [[ "$TOOLNAME" == "images" ]]
    then
        echo "in if"
        continue
    fi
    for VIDFILE in $TOOLDIR/*
    do
        echo "\t${VIDFILE}"
        VIDNAME="$(basename ${VIDFILE})"
        VIDNAME="$(echo $VIDNAME | sed s/.*-//)"
        echo "\t\t${VIDNAME%.*}"
        ffmpeg -i $VIDFILE "${IMAGEDIR}/${TOOLNAME}__${VIDNAME%.*}_%09d.png"
    done
done
