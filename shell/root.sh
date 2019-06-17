#!/bin/bash
test=$(env | grep USER | cut -d "=" -f 2)
echo $test

if [ "$test" == "root" ]
    then
        echo "user is root"
fi