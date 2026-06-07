#!/bin/bash

rm -rf build/
cmake -B build -DIMPLEMENTATION=ADJACENCY_LIST -DCMAKE_BUILD_TYPE=Debug
cmake --build build
