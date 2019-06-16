#!/bin/bash
cd /Users/lyn
ls *.tar.gz > ls.log

for fileName in $(cat ls.log)
    do
        tar -zxvf $fileName &> /dev/null
        rm -rf $fileName
    done

rm -rf ls.log