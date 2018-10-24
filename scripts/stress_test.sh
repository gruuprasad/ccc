#!/usr/bin/env bash
cd ..
NAME=c4 CFG=release make
cd build

# Only for linux :/
# perf stat -r 10 -B bash ../scripts/time_all_lexer.sh
HEADER="Wall\t|Sys\t|Usr\t|CPU\t|Mem\t|hard page\t|soft page\t|swap\t|ctx\t|wait\n"
FORMAT="%E\t|%S\t|%U\t|%P \t|%M\t|%F\t\t\t|%R\t\t|%W\t\t|%c\t\t|%w\t %C"
printf "${HEADER}"
sleep 0.1
/usr/bin/time -f "${FORMAT}" ../scripts/time_all_lexer.sh --tokenize
sleep 0.1
echo "------------------------------------"
sleep 0.1
/usr/bin/time -f "${FORMAT}" ../scripts/time_all_lexer.sh --tokenize-old
sleep 0.1
#/usr/bin/time --verbose ../scripts/time_all_lexer.sh
