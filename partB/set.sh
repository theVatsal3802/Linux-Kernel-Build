#! usr/bin/bash

sudo rmmod set
make clean
make
gcc user.c
sudo insmod ./set.ko
./a.out
