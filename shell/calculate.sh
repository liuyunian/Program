#!/bin/bash
read -t 30 -p "please enter the operation expression " num1 opt num2
# echo "num1 = $num1, opt = $opt, num2 = $num2"
if [ -n "$num1" -a -n "$opt" -a -n "$num2" ]
then
    test1=$(echo $num1 | sed 's/[0-9]//g')
    test2=$(echo $num2 | sed 's/[0-9]//g')
    if [ -z $test1 -a -z $test2 ]
    then
        if [ "$opt" == "+" ]
        then 
            result=$(($num1 + $num2))
        elif [ "$opt" == "-" ]
        then
            result=$(($num1-$num2))
        elif [ "$opt" == "*" ]
        then
            result=$(($num1*$num2))
        elif [ "$opt" == "/" ]
        then
            result=$(($num1/$num2))
        else
            echo "please enter valid operator"
            exit 12
        fi
    else
        echo "please enter valid number"
        exit 11
    fi
else
    echo "please enter three non-empty parameters"
    exit 10
fi

echo "$num1 $opt $num2 = $result"