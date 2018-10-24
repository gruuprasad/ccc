#!/usr/bin/env bash
HEADER="Wall\t|Sys\t|Usr\t|CPU\t|Mem\t|hard page\t|soft page\t|swap\t|ctx\t|wait\n"
FORMAT="%E\t|%S\t|%U\t|%P \t|%M\t|%F\t\t\t|%R\t\t|%W\t\t|%c\t\t|%w\t %C"
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/test.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/comments.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/everything.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/extra.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/hello_world.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/10k.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/10kops.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kops.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kmixed.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kmixstring.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kbraces.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kconst.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kmoreconst.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kmoreconsts.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kmix.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100kkw.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100knumbers.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100knumber.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100k.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/1000k.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/1000kv2.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/10m.c > /dev/null
/usr/bin/time -f "${FORMAT}" ./release/c4 $1 ../examples/100m.c > /dev/null
sleep 0.001
true
