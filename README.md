# Advances in Operating System Design Course Assignment 1

## Submitted By:
- Vatsal Adhiya (24CS60R26)
- Himanshu Likhar (24CS60R43)

# Part 1: Linux 5.10.223 Build

## Requirements as per assignment
- Remove IPv6 Protocol
- Make Kernel based Virtual Machine(KVM) an inbuilt feature instead of a kernel module.

## Steps
- Download the kernel from the source [here](https://kernel.org/#:~:text=longterm%3A-,5.10.223,-2024%2D07%2D27). Download the tarball.
- Unzip the tarball:
```
tar -xvf linux-5.10.223.tar.xz
```
- Update Ubuntu and install required dependencies
```
sudo apt update && sudo apt upgrade -y
sudo apt install build-essential libncurses-dev bison flex libssl-dev
```
This will update the programs to their latest versions and then install the mentioned dependencies
- Navigate into the linux folder just unzipped and open the folder in terminal.
- Execute the following command:
```
sudo make menuconfig
```
This will open a configuration menu as shown: <br>
<div align="center">
  <img src="https://github.com/user-attachments/assets/a20a4664-b5b8-4cbe-a836-b9dbde30d761" width="70%">
</div>

### To disable IPv6 Protocol
- Navigate to the <b>Networking support ---> </b>
- Navigate to the <b>Networking options ---> </b>
- Find <b>The IPv6 Protocol</b> in the displayed list.
- To disable it, press ```Shift + N``` and Save the file and exit. (Refer the image below)
<div align="center">
  <img src="https://github.com/user-attachments/assets/8b0e6468-1664-463b-9ffb-a130010f37f6" width="70%">
</div>


### To make KVM a inbuilt feature from a module feature
- Navigate to the <b>Virtualization</b>
- Find <b>Kernel-based Virtual Machine (KVM) support</b>
- To make it a built-in feature, press ```Shift + Y```. (Refer the image below)
- Save the file and exit.
<div align="center">
  <img src="https://github.com/user-attachments/assets/36498e06-a90d-401b-9478-485e92f3b6f7" width="70%">
</div>

### Save the configurations
- When we exit the menuconfig menu, the editted configurations are saved to the <b><em>.config</em></b> file. (Refer the image below)
<div align="center">
  <img src="https://github.com/user-attachments/assets/dd973bb2-1e82-4948-abcb-df73fa99db9d" width="70%">
</div>

### Make minor changes in configurations
- After saving the <b><em>.config</em></b> file, open the file in any text editor:
```
sudo gedit .config
```
- Make the following changes:
    - Search <b>CONFIG_SYSTEM_TRUSTED_KEY</b> and set it's value to empty string.
    - Search <b>CONFIG_SYSTEM_REVOCATION_KEY</b> and set it's value to empty string.
    - Search <b>CONFIG_MODULE_SIG_KEY</b> and set it's value to empty string.
    - Search <b>CONFIG_MODULE_SIG</b> and set it's value to `n`.
    - Search <b>CONFIG_MODULE_SIG_ALL</b> and set it's value to `n`.

### Compiling the Kernel
- After saving the file, next we have to compile the kernel with the following command:
```
sudo make -j 8
```
The -j 8 is the number of parallel threads to be used just to speed up the compilation process.

### Install the kernel and modules
- After the compilation is complete, we have to install the modules and the kernel. We use the following command for the installations:
```
sudo make modules_install -j 8 && sudo make install -j 8
```
The above 2 commands can be run interchangeably and will have the same effect.

### Update Grub
- Grub menu is actually automatically updated when the kernel is installed, but can be manually updated using the following command:
```
sudo update-grub
```

## Reboot the system
-  Select the newly installed linux kernel from the Grub menu, and the system will boot using the new kernel.
-  To verify if the system is booted in the new kernel, run the following command:
```
uname -r
```
If the version of the kernel is same as the installed kernel, the installation was successful.

### Verify the changes made in the configuration file
- Run the following command to check if IPv^ protocol is disabled or not:
```
ip -6 address
```
If there is no output, it means that the IPv6 protocol was successfully disabled.
- Run the following command to check if KVM is present or not:
```
ls /dev/kvm
```
If there is a output other than no such file or directory, it means that KVM was successfully installed. <br>
The below image shows the verification steps
<div align="center">
  <img src="https://github.com/user-attachments/assets/9e2ee5e4-70b5-4a17-b52a-24d3f8d5cbc2" width="70%">
</div>

## Changes observed
We observe the follwoing changes after installing the new kernel:
- The IPv6 protocol is no longer being used for connections.
- KVM virtualization is enabled as an in-built feature instead of a module feature.
