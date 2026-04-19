#!/bin/bash

rm -rf build/
cmake -G Ninja -B build -DADJACENCY_LIST=ON
cmake --build build
