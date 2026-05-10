#!/bin/bash

rm -rf build/
cmake -G Ninja -B build -DADJACENCY_MATRIX=ON
cmake --build build
