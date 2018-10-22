#!/usr/bin/env bash

lcov --directory . --capture --output-file coverage.info &> /dev/null
lcov --remove coverage.info '/usr/bin/*' --output-file coverage.info  &> /dev/null
lcov --remove coverage.info '/usr/lib/*' --output-file coverage.info  &> /dev/null
lcov --remove coverage.info '/usr/include/*' --output-file coverage.info  &> /dev/null
lcov --remove coverage.info '/usr/sbin/*' --output-file coverage.info  &> /dev/null
lcov --remove coverage.info '/usr/share/*' --output-file coverage.info  &> /dev/null
lcov --remove coverage.info 'test/*' --output-file coverage.info  &> /dev/null
lcov --remove coverage.info 'src/reflex/*' --output-file coverage.info  &> /dev/null
lcov --remove coverage.info 'src/lexer/reflex_lexer.l' --output-file coverage.info  &> /dev/null
lcov --list coverage.info \
        | grep "Total:" \
        | sed "s/\s*Total:\|\(\d+\.\d+\%\).*/\1/" \
        | cut -d "|" -f 2 \
        | cut -d " " -f 1 \
        | xargs echo "coverage found is $1"

genhtml coverage.info --output-directory ../coverage

true
