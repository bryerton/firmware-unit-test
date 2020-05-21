#!/bin/sh

# Expects to be run from the directory it is located in
cd "$(dirname "$(realpath "$0")")";

# Generate coverage reports and place them in report directory
gcovr -r . -f '/.*/src' -e '.*\.h' --html --html-details -o report/coverage.html -s
