#! /usr/bin/bash

rm set*.txt
sudo rmmod set
make clean
make
gcc user.c
sudo insmod ./set.ko
