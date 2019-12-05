# Multi-threaded Square Matrix Multiplication

## Table of contents
* [Overview](#overview)
* [Prerequisites](#prerequisites)
* [Getting Started](#getting-started)
* [Building](#building)
* [Side Note](#side-note)
* [Motivation](#motivation)
* [Times Measured](#times-measured)
* [Status](#status)
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
* -t is an optional argument which takes in a number greater than 0 which represents the number threads to be created to perform the matrix multiplication. When -t is not specified, the number of threads is set to 1.    
* -o is an optional argument which takes in the name of the file in which the output matrix is to be stored. If this flag is not specified, the output matrix is discarded.
```
./m6 matrix1 matrix2 [-t <number of threads>] [-o <output file name>]
```

### Example
* Specifying the -t flag to specify the number of threads to be used for computing the values and -o flag to save the output of the matrices in a file:
```
./m6 matrix1 matrix2 -t 10 -o output.dat
``` 

## Side Note
* When the number of the threads is greater than dimension * dimension, the  number of threads to be created is set to dimension * dimension. This is to avoid creation of extra threads which do not perform any tasks.
* Before creating any child threads, the parent thread calculates the number of row and column operations to be performed by each thread and those values in a struct created for each thread. 
* Now the child threads are created and compute the rows and columns they are given. 
* By doing so, there is no requirement of synchronization as every thread computes the rows and columns given which leads to a highly concurrent program with no possible race conditions.
* In my opinion, this approach is "better" than using synchronization because of the following reasons:
	* All the threads waiting to acquire the lock have to keep waiting until the lock is released or the thread holding the lock is descheduled. If a thread holding a lock is delayed (due to a page fault, scheduling delay, etc.), then no thread that needs that lock can make progress. This leads to a waste of time during waiting which can be avoided by following the approach implemented in my assignment.
	* Use of locks adds an overhead for each access to a resource even when the chances of having a race condition is very low. This overhead can be avoided by designing the program in a way that each thread is given the work they need to perform without having any overlap.

## Motivation
* If you want to test the time difference in computation with different number of threads and matrices, you can run the following command: 
```
make generateData
```
* This command runs a bash script to generate the data by calculating the computation time for multiplying the identity matrices of dimensions- 10, 100, 500, 1000, 5000, 10000 and different number of threads- 10, 20, 40, 80, 120, 160. 
* The time taken for computation for each file and thread is stored in a text file.
* This text file is now used by a python script which plots a graph and displays it.

* If you have the data file, for example: ./graphs/testResults.txt (in my repository) and you wish to plot it, you can run the following command to run a python script:
```
make plot
```

* If your graph data file is named differently and you wish to plot it, you can directly run the python script by:
```
python3 ./graphs/graphGenerator.py <fileName>
```

## Times Measured
For the following files and number of threads, the time results (in seconds) are given the following table:

<table style="width:100%">
  <tr><th>File Name</th> <th>160 Threads</th><th>120 Threads</th><th>80 Threads</th><th>40 Threads</th><th>20 Threads</th><th>10 Threads</th><th>1 Thread</th></tr>
  <tr><td>m10.dat</td> <td>0.0185410340</td><td>0.0176446500</td><td>0.0164358010</td><td>0.0129977520</td><td>0.0108341230</td><td>0.0098444360</td><td>0.0095271970</td></tr>

  <tr><td>m100.dat</td> <td>0.0179238348</td><td>0.0174436940</td><td>0.0131863240</td><td>0.0128605950</td><td>0.0114229760</td><td>0.0120185290</td><td>0.0200017910</td></tr>

  <tr><td>m500.dat</td> <td>0.0410683900</td><td>0.0408631890</td><td>0.0395425950</td><td>0.0479200240</td><td>0.0490849880</td><td>0.0712629350</td><td>0.4832405140</td></tr>

  <tr><td>m1000.dat</td> <td>0.1501630190</td> <td>0.1458612020</td><td>0.1532011420</td><td>0.2602762910</td><td>0.2406372030</td><td>0.4335066180</td><td>3.9594505950</td></tr>

  <tr><td>m5000.dat</td> <td>20.9170149750</td><td>21.9774168850</td><td>21.0494958910</td><td>24.8996945430</td><td>48.9794775770</td><td>97.7685682500</td><td>1014.6978075420</td></tr>

  <tr><td>m10000.dat</td> <td>304.4820270940</td><td>307.7789388800</td><td>241.2476528310</td><td>261.0176836150</td><td>513.7659469080</td><td>1025.0914597390</td><td>10521.0287326060</td></tr>

</table>

* The tests were run on a server with 40 physical cores and 80 cores with hyperthreading, there is not a huge increase in performance when the thread count exceeds 40. 
* In case of matrix of dimension 10000, having more threads than cores seems to be an overhead as it keeps context switching between those threads to perform the task.
* As expected, the table shows huge decrease in times measured when the thread count goes up (in cases of 1, 10, 20 and 40 threads). This is noticeable in the matrices with large dimensions.
* These numbers are recorded by the bash script which stores the numbers in a file 'testResults.txt' which is in the 'graphs' directory.
* The file "allFiles.png" would represent a graph for having all the test files given with different number of threads.
* Individual graphs associated with each file can also be found in the directory 'graphs'.

## Status
The project is finished.

## Authors
* Vasu Gupta 
