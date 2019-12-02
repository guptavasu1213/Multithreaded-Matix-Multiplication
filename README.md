-----------------------------------------------------------------
Name: Vasu Gupta
Student ID: 3066521
Assignment 3
CMPT 360
----------------------------------------------------------------


# Process Manager

## Table of contents
* [Overview](#overview)
* [Prerequisites](#prerequisites)
* [Getting Started](#getting-started)
* [Building](#building)
* [Side Note](#side-note)
* [Status](#status)
* [Motivation](#motivation)
* [Authors](#authors)

## Overview

This is a multi-threaded program that multiplies square matrices. The matrices are stored in binary files which contains 32-bit integers. 

## Prerequisites

You need to have GCC to compile this file.
This program works on the gcc version (SUSE Linux) 4.8.5

To check if GCC is installed, run the following command on the Linux/UNIX terminal:
```
gcc --version
```

## Getting Started
* Clone/download this repository in a local computer
* Create an executable by running the following target from the makefile:
```
make all
```

## Building
### Usage
* Matrices to be multiplied are passed as an argument
* There are two known flags which can be passed when running the program
* -t is a required argument which takes in a number greater than 0 which represents the number threads to be created to perform the matrix multiplication.
* -o is an optional argument which takes in the name of the file in which the output matrix is to be stored. If this flag is not specified, the output matrix is discarded.
```
./m6 matrix1 matrix2 -t <number of threads> [-o <output file name>]
```

### Example
* Specifying the -t flag to specify the number of threads to be used for computing the values and -o flag to save the output of the matrices in a file:
```
./m6 matrix1 matrix2 -t 10 -o output.dat
``` 

## Side Note
* Before creating any child threads, the parent thread calculates the number of row and column operations to be performed by each thread and those values in a struct created for each thread. 
* Now the child threads are created and compute the rows and columns they are given. 
* By doing so, there is no requirement of synchronization as every thread computes the rows and columns given.
* In my opinion, this approach is "better" than using synchronization because of the following reasons:
- All the threads waiting to acquire the lock have to keep waiting until the lock is released or the thread holding the lock is descheduled. If a thread holding a lock is delayed (due to a page fault, scheduling delay, etc.), then no thread that needs that lock can make progress. This leads to a waste of time during waiting which can be avoided by following the approach implemented in my assignment.
- Use of locks adds an overhead for each access to a resource even when the chances of having a race condition is very low. This overhead can be avoided by designing the program in a way that each thread is given the work they need to perform.

## Motivation
* If you want to test the time difference in computation with different number of threads and matrices, you can run the following command: 
```
make generateData
```
* This command runs a bash script to generate the data by calculating the computation time for multiplying the identity matrices of dimensions- 10, 100, 500, 1000, 5000, 10000 and different number of threads- 10, 20, 40, 80, 120, 160. 
* The time taken for computation for each file and thread is stored in a text file.
* This text file is now used by a python script which plots a graph and displays it.

* If you have the data file, for example: graph.txt (in my repository) and you wish to plot it, you can run the following command to run a python script:
```
make plot
```

* If your graph data file is named differently and you wish to plot it, you can directly run the python script by:
```
python3 graphGenerator.py <fileName>
```

## Status
The project is finished.

## Authors
* Vasu Gupta 
