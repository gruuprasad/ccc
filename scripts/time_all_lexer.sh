#!/usr/bin/env bash

./release/c4 --tokenize ../examples/test.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/comments.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/everything.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/extra.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/hello_world.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/10k.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/100k.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/1000k.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/1000kv2.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/10m.c > /dev/null && echo -n '.'
./release/c4 --tokenize ../examples/100m.c > /dev/null && echo -n '.'

true
