# Loadable Kernel Module Development

In this part of the assignment, we developed a Loadable Kernel Module (LKM) and installed it in the Linux kernel.

## Steps
- Download the code from: https://github.com/theVatsal3802/Linux-Kernel-Build.git
- Extract the zip and open the folder in ternimal and navigate to the `partB` folder.
- Run the bash script called `set.sh` using the following command:
```
$ ./set.sh
```
This script will perform the following tasks:
- Remove a module named `set` (if it already exists).
- Compile the LKM using the `make` command
- Compile the user space process program using the command `gcc user.c` and generate a `a.out` executable.
- Insert the LKM using the `insmod` command.

Now you have two options:
- Option 1: Run `proc.sh`. This script will create 50 processes. For each process, it will generate a random number between 1 and 100 (both inclusive), let us call it `n`. Next for `i-th` process, it inserts `n` multiples of `i` in the set (for that process). We have not inserted duplicates using this script so that functionality (duplicate checking) will not be tested using this script. The output for `i-th` file will be written in `process_<i>_output.txt`, in the same directory.
- Option 2: Run `a.out` manually. You can run it on as many terminals (<=100) as you want. 

## Features
- The Set functionality of the LKM is implemented using **AVL Tree** which makes it optimal.
- The LKM handles concurrency.
- No user space program will be able to open the file more than once simultaneously.
- Maximum 100 processes can have active sets. It can be changed by changing the `MAX_SET_SIZE` parameter at the start of `set.c`.

## FAQs:
**Q**: I cannot run either of `set.sh` or `proc.sh`. What do I do?<br>
**A**: Ensure your linux user has the rights to execute the executable. Use `chmod` to provide required permissions. Visit [here](https://www.linux.org/threads/file-permissions-chmod.4124/) for more info.<br>
**Q**: Module Compilation failed.<br>
**A**: We tested it on multiple kernels but the only errors we encounterd were:
- Your system's gcc version and the version used for compiling kernel are diff. If you encounter this, just install the version it says in the error. Refer [this](https://askubuntu.com/questions/1500017/ubuntu-22-04-default-gcc-version-does-not-match-version-that-built-latest-defaul) for more info.
- Linux header are not present. For this ensure that you have all the necessary libraries and  headers using:
  ```
  $ sudo apt update
  $ sudo apt install build-essential libncurses-dev libssl-dev libelf-dev bison flex -y
  $ sudo apt install linux-headers-$(uname -r)
  ```
If you encounter any more issues, please reach out to us at: [Himanshu Likhar](mailto:himanshu.likhar.24@kgpian.iitkgp.ac.in), [Vatsal Adhiya][email-here]
