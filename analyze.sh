#!/bin/bash

if [ $# -eq 1 ];
then
    # Your code here. (1/4)i
	grep -E 'WARN|ERROR' $1 > bug.txt
else
    case $2 in
    "--latest")
        # Your code here. (2/4)
	tail -n 5 $s1
    ;;
    "--find")
        # Your code here. (3/4)
	grep $s3 $1 > $3.txt
    ;;
    "--diff")
        # Your code here. (4/4)
	
    ;;
    esac
fi
