#!/usr/bin/env bash

for dir in ./*/
do
  if [ -d "$dir" ]
  then
    cd "$dir" || exit 1
    make
    make clean
    cd ..
  fi
done
