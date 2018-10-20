#!/usr/bin/env bash

gcovr --object-directory="./" --root=./ --gcov-exclude=".*tests.*" --gcov-exclude="/usr/include/.*"
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/bin/*' --output-file coverage.info # filter out system
lcov --remove coverage.info '/usr/lib/*' --output-file coverage.info # filter out system
lcov --remove coverage.info '/usr/include/*' --output-file coverage.info # filter out system
lcov --remove coverage.info '/usr/sbin/*' --output-file coverage.info # filter out system
lcov --remove coverage.info '/usr/share/*' --output-file coverage.info # filter out system
lcov --remove coverage.info 'test/*' --output-file coverage.info # filter out test
lcov --list coverage.info
genhtml coverage.info --output-directory ../coverage

true
