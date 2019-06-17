#!/bin/bash
test=$(df -h | grep /dev/disk1s1 | awk '{print $5}' | cut -d "%" -f 1)
echo $test

if [ $test -ge 20 ]
    then
        echo "/ is full"
fi