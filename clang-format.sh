#!/bin/bash
find ./ -name '*.h' -exec clang-format -i '{}' \;
find ./ -name '*.cpp' -exec clang-format -i '{}' \;
