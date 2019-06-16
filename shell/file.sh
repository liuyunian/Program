#!/bin/bash

read -p "please input a filePath: " -t 30 filePath
echo $filePath

if [ -e "$filePath" ]
    then
        echo "$filePath is exist"
    else
        echo "$filePath is not exist"
fi