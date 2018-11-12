#!/usr/bin/env bash
HEADER="Wall\t|Sys\t|Usr\t|CPU\t|Mem\t|hard page\t|soft page\t|swap\t|ctx\t|wait\n"
FORMAT="%E\t|%S\t|%U\t|%P \t|%M\t|%F\t\t\t|%R\t\t|%W\t\t|%c\t\t|%w\t %C"
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/declarations.c > /dev/null
sleep 0.001
true
