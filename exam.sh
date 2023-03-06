#!bin/bash
mkdir mydir
chmod +7 mydir
echo 2023 > myfile
mv moveme mydir
mv copyme copied
gcc bad.c 2>> err.txt


mkdir gen
cd gen
a=$[$1]
if [ $a -eq 0 ]; then a=10; fi 
while [ $a -gt 0 ]
do 
	mkdir $a.txt
	let a--
done


