#!/bin/bash
read -t 30 -p "please enter a fileName: " fileName
echo $fileName

if [ -n "$fileName" ]
then
    if [ -e "$fileName" ]
    then
        if [ -f "$fileName" ]
        then 
            echo "INFO: $fileName is a general file"
        elif [ -d "$fileName" ]
        then
            echo "INFO: $fileName is a directory"
        else
            echo "INFO: $fileName is a other type file"
        fi
    else
        echo "INFO: $fileName is not exist"
    fi
else
    echo "ERROR: fileName is empty"
fi