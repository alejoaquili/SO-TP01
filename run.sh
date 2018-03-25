#!/bin/bash

echo 'Insert the name of the files to hash (file1 file2 file3 ... filen):'

read var

program='./Application/applicationProcess.out '$var

echo $program

eval $program
