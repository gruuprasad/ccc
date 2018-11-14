#!/usr/bin/env bash

cd ..
CFG=release make clean
NAME=c4 CFG=release make

nice -n -20 python3 ./scripts/stats.py ./build/release/c4 ./build/legacy
