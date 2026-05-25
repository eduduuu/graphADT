#!/bin/bash

rm -rf build/
cmake -G Ninja -B build -DIMPLEMENTATION=ADJACENCY_LIST
cmake --build build
