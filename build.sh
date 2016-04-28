#!/bin/sh

BUILD_FOLDER=build

if [ -d "$BUILD_FOLDER" ]; then
  rm -rf $BUILD_FOLDER
fi

mkdir $BUILD_FOLDER
cd $BUILD_FOLDER
cmake -DCMAKE_BUILD_TYPE=DEBUG ..
make

