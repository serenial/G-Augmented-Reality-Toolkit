#!/usr/bin/env bash
cd "$(dirname "$0")"

BUILD_TYPE="release"


if [ $# -eq 0 ];
  then
     echo "*** Building release ***"
     echo "If a debug version is required supply the batch file with the argument \"debug\"."
elif [[ "$1" -eq "debug" ]];
    then
    echo "** Building debug ***"
    BUILD_TYPE="debug"
fi

cmake --preset=linux-x64-$BUILD_TYPE && cmake --build --preset=linux-x64-$BUILD_TYPE
