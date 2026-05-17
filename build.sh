#!/bin/bash

rm -rf build/
cmake -G Ninja -B build -DIMPLEMENTATION=CSR
cmake --build build
