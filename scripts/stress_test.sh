#!/usr/bin/env bash
cd ..
NAME=c4 CFG=release make
cd build

# Only for linux :/
# perf stat -r 10 -B bash ../scripts/time_all_lexer.sh

time ../scripts/time_all_lexer.sh
