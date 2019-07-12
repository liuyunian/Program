#!/bin/bash
basePath=$(pwd)
subdirectory=$(ls -F $basePath | grep "/")
# echo $subdirectory
for dir in "" $subdirectory
    do
        for filePath in $basePath/$dir
            do
                # echo $filePath
                rm -f $filePath/*.out
                rm -f $filePath/*.o
            done
    done