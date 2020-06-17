#!/usr/bin/bash

if [ $# -ne 1 ];then
	echo "Invalid arguments. usage: $0 <outputFileName>"
	exit 1	
fi

graphFileName=$1
> $graphFileName #Creating file

make all

# This check is only valid if identity matrices are multiplied
check(){
	diff $1 $2
	if [ $? -ne 0 ]; then
		echo "The multiplication had an error!"
		exit 1
	fi
}
for fName in m10.dat m100.dat m500.dat m1000.dat m5000.dat m10000.dat; do
	printf "$fName:" >> $graphFileName
	for numThreads in 160 120 80 40 20 10 1; do 
		echo -e "\n===== TESTING FOR FILE $fName for $numThreads threads. =====\n"

		startTime=$(date +%s%N)
		./m6 './inputMatrices/'$fName './inputMatrices/'$fName -t $numThreads -o outFile.txt 
		endTime=$(date +%s%N)

		printf "Time elapsed: %.10f\n" "$((endTime-startTime))e-9"
		check './inputMatrices/'$fName "outFile.txt"
		printf "$numThreads=%.10f," "$((endTime-startTime))e-9" >> $graphFileName
	done
	echo "" >> $graphFileName
done
