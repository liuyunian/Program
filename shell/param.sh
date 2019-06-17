#!/bin/bash
# echo "param is $*"
# echo "param is $@"
# echo "param num is $#"

for i in "$*"
    do
        echo $i
    done

for j in "$@"
    do
        echo $j
    done