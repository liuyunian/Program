#!/bin/bash
read -t 30 -p "please enter yes or no: " choice
echo $choice

case $choice in
    "yes")
        echo "INFO: your choice is yes"
        ;;
    "no")
        echo "INFO: your choice is no"
        ;;
    *)
        echo "INFO: your enter is error"
        ;;
esac

    