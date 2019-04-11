#!/bin/sh
module load gcc/5.3.0
gcc -fopenmp -std=c99 -o adapt adapt.c -lm
