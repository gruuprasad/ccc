#!/usr/bin/env bash

cd ..
NAME=c4 CFG=release make

python3 ./scripts/stats.py ./build/release/c4
