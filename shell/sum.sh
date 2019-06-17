#!/bin/bash
sum=0
for ((i=1; i<=100; i=i+1))
    do 
        sum=$(($sum+$i))
    done

echo "the result is $sum"