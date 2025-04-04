# SimpleThreading

a simple multithreading program to count amount of integers below 64 in a 2D array with size of 8192 x 8192, each cell filled with a random integer from 0 to 255
the 2D array is split into multiple chunks to be processed by a thread each, the number of threads depending on thread::hardware_concurrency()
the execution time is then compared with the same program without multithreading
