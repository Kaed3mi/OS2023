#!/bin/bash

if [ $# -eq 1 ];
then
	grep -E 'WARN|ERROR' $1 > bug.txt
    # Your code here. (1/4)i

else
    case $2 in
    "--latest")
        # Your code here. (2/4)

    ;;
    "--find")
        # Your code here. (3/4)

    ;;
    "--diff")
        # Your code here. (4/4)

    ;;
    esac
fi
