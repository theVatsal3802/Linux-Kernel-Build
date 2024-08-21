# Loadable Kernel Module Development

In this part of the assignment, we developed a Loadable Kernel Module (LKM) and installed it in the Linux kernel.

## Steps
- Clone this repo
```
git clone https://github.com/theVatsal3802/Linux-Kernel-Build.git
```
- Open the folder in ternimal and navigate to the `partB` folder:
```
cd partB/
```

- Run the bash script called `set.sh` using the following command:
```
bash set.sh
```
This script will perform the following tasks:
- Compile the LKM using the `make` command
- Compile the user space process program using the command `gcc user.c`
- Install the LKM using the `insmod` command
- Run the LKM using the `./a.out` command

## Features
- The Set functionality of the LKM is implemented using AVL Tree which makes it optimal.
- The LKM handles concurrency.
- No user space program will be able to open the file more than once simultaneously.
