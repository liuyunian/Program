#!/bin/bash
sum=0
i=1
until [ "$i" -gt 100 ]
    do
        sum=$(($sum+$i))
        i=$(($i+1))
    done

echo "the result is $sum"