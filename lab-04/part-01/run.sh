#!/bin/bash

cd term-paper
./drop-simulator -window meta/sphere.txt meta/coefs.txt & _pid=$! # launch ur kursach
cd ..
gcc main.c helpers.c -o app
./app ${_pid}