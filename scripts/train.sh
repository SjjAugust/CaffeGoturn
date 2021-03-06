#!/bin/bash

# if [ -z "$4" ]
#   then
#     echo "No folder supplied!"
#     echo "Usage: bash `basename "$0"` imagenet_folder imagenet_annotations_folder alov_videos_folder alov_annotations_folder"
#     exit
# fi

GPU_ID=0
FOLDER=GOTURN1
RANDOM_SEED=800

echo FOLDER: $FOLDER

VIDEOS_FOLDER_IMAGENET=/userhome/shijj/GOTURN/data/VisDrone2019-DET-train/JPEGImages
ANNOTATIONS_FOLDER_IMAGENET=/userhome/shijj/GOTURN/data/VisDrone2019-DET-train/Annotations
VIDEOS_FOLDER=/userhome/shijj/GOTURN/data/VisDrone2019-SOT-train/sequences
ANNOTATIONS_FOLDER=/userhome/shijj/GOTURN/data/VisDrone2019-SOT-train/annotations
SOLVER=/userhome/shijj/GOTURN/nets/solver.prototxt
TRAIN_PROTO=/userhome/shijj/GOTURN/nets/tracker.prototxt
CAFFE_MODEL=/userhome/shijj/GOTURN/nets/models/weights_init/tracker_init.caffemodel

BASEDIR=/userhome/shijj/GOTURN/nets
RESULT_DIR=$BASEDIR/results/$FOLDER
SOLVERSTATE_DIR=$BASEDIR/solverstate/$FOLDER

#Make folders to store results and snapshots
mkdir -p $RESULT_DIR
mkdir -p $SOLVERSTATE_DIR

#Modify solver to save snapshot in SOLVERSTATE_DIR
mkdir -p /userhome/shijj/GOTURN/nets/solver_temp
SOLVER_TEMP=/userhome/shijj/GOTURN/nets/solver_temp/solver_temp_$FOLDER.prototxt
sed s#SOLVERSTATE_DIR#$SOLVERSTATE_DIR# <$SOLVER >$SOLVER_TEMP
sed -i s#TRAIN_FILE#$TRAIN_PROTO# $SOLVER_TEMP
sed -i s#DEVICE_ID#$GPU_ID# $SOLVER_TEMP
sed -i s#RANDOM_SEED#$RANDOM_SEED# $SOLVER_TEMP

LAMBDA_SHIFT=5
LAMBDA_SCALE=15
MIN_SCALE=-0.4
MAX_SCALE=0.4

echo LAMBDA_SCALE: $LAMBDA_SCALE
echo LAMBDA_SHIFT: $LAMBDA_SHIFT

/userhome/shijj/GOTURN/build/train $VIDEOS_FOLDER_IMAGENET $ANNOTATIONS_FOLDER_IMAGENET $VIDEOS_FOLDER $ANNOTATIONS_FOLDER $CAFFE_MODEL $TRAIN_PROTO $SOLVER_TEMP $LAMBDA_SHIFT $LAMBDA_SCALE $MIN_SCALE $MAX_SCALE $GPU_ID $RANDOM_SEED 2> $RESULT_DIR/results.txt
