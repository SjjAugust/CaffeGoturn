#!/bin/bash

if[$# != 4]
    then
    echo "Usage: sh basename/$0 video_folder annotations_floder caffe_model output_folder"
    exit
fi

if[-z $1]
    then
        echo "No folder supplied!"
        echo "Usage: sh basename/$0 video_folder annotations_floder caffe_model output_folder"
        exit
fi

GPU_ID=0

# Whether to evaluate on the training set or the validation set,always 0
USE_TRAIN=0

# Whether or not to save videos of the tracking output,always 1
SAVE_VIDEOS=1

VIDEOS_FOLDER=$1

ANNOTATIONS_FOLDER=$2

DEPLOY_PROTO=nets/tracker.prototxt

CAFFE_MODEL=$3

OUTPUT_FOLDER=$4

build/test_tracker_visdrone $VIDEOS_FOLDER $ANNOTATIONS_FOLDER $DEPLOY_PROTO $CAFFE_MODEL $OUTPUT_FOLDER $USE_TRAIN $SAVE_VIDEOS $GPU_ID

echo "Saving output to $4"
