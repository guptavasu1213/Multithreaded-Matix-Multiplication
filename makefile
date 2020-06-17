# /*-----------------------------------------------------------------
# Name: Vasu Gupta
# Student ID: 3066521
# Assignment 3
# CMPT 360
# *----------------------------------------------------------------*/
CC=gcc
CFLAGS= -Wall -g 

graphFileName=./graphs/testResults.txt

all: m6

m6: m6.c
	$(CC) $(CFLAGS) -o m6 m6.c -lpthread -lm 
testrace:./inputMatrices/
	valgrind --tool=helgrind ./m6 ./inputMatrices/m100.dat ./inputMatrices/m100.dat -t 20 -o output.dat
testleaks:
	valgrind --leak-check=full --show-leak-kinds=all -v ./m6 ./inputMatrices/m100.dat ./inputMatrices/m100.dat -t 20 -o output.dat
checkp:
	/tmp/cmpt360/checkpatch.pl --no-tree --file --terse m6.c 
generateData:
	bash automateCalculations.sh ${graphFileName}
	plot
plot:
	python3 ./graphs/graphGenerator.py ${graphFileName}