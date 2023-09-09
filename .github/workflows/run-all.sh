#!/usr/bin/env bash

for dir in ./*/
do
  if [ -d "$dir" ]
  then
    cd "$dir" || exit 1
    make bin
    make clean
    cd ..
  fi
done
