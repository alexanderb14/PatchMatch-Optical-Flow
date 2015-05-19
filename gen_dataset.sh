#!/bin/bash

#
# Author: Alexander Brauckmann
#

# This script runs the patchmatch implementation on the middlebury dataset
# (http://vision.middlebury.edu/flow/data/)

#config
PATCHMATCH_EXEC=./PatchMatchBenchmark
FLOWCODE_EXEC=./../../middlebury/flow-code/color_flow
COLORTEST_EXEC=./../../middlebury/flow-code/colortest

DATASET_DIR=../../middlebury/trainingData/other-data/*;
GROUNDTRUTH_DIR=../../middlebury/trainingData/other-gt-flow/*;

#parameters
SIMILARITY_MEASURES=(
  "SumOfSquaredDifferences" 
  "CrossCorrelationNormalized")
COLOR_RANGE=10

#clean up and create env for output
for f in $DATASET_DIR;
do
  for i in ${SIMILARITY_MEASURES[@]}
  do
    rm -r $f/$i
    mkdir $f/$i
  done
done

#fork program and store pids in array
PID_COUNTER=0
for f in $DATASET_DIR;
do
  for i in ${SIMILARITY_MEASURES[@]}
  do
    echo "Processing $f data set with similarity measure $i..." &

    $PATCHMATCH_EXEC $f/frame10.png $f/frame11.png $f/$i $i 2 &
    pids[${PID_COUNTER}]=$!
    let PID_COUNTER=PID_COUNTER+1
  done
done

#wait for all forked processes to finish
for pid in ${pids[*]}; 
do 
  wait $pid; 
done;

#compute flow images
for f in $DATASET_DIR;
do
  for i in ${SIMILARITY_MEASURES[@]}
  do
    echo "Compute flow images in $f with similarity measure $i..."

    for t in $f/$i/*;
    do
      $FLOWCODE_EXEC $t $t.png $COLOR_RANGE
    done

#    rm $f/$i/*.flo
  done
done

#generate groundTruth images
for f in $GROUNDTRUTH_DIR;
do
  echo "Compute groundtruth image in $f..."

  for t in $f/*;
  do
    $FLOWCODE_EXEC $t $t.png $COLOR_RANGE
  done
done
