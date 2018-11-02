#!/usr/bin/env bash
cd ..
NAME=c4 CFG=release make
cd build/
./tests
dot ast.gv -Tsvg > ast.svg
