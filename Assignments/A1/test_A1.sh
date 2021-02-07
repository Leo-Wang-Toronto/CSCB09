#!/bin/sh
gcc -Wall -std=c99 -O3 -o classifier classifier.c knn.c -lm
./classifier 1 lists/training_1k.txt lists/testing_1k.txt
./classifier 2 lists/training_1k.txt lists/testing_1k.txt
./classifier 4 lists/training_1k.txt lists/testing_1k.txt
./classifier 8 lists/training_1k.txt lists/testing_1k.txt
./classifier 16 lists/training_1k.txt lists/testing_1k.txt
