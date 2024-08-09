# Linux 5.10.223 Build

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
This will open a configuration menu as shown:

# Insert image here

### To disable IPv6 Protocol
- Navigate to the <b>Networking support ---> </b>
- Navigate to the <b>Networking options ---> </b>
- Find <b>The IPv6 Protocol</b> in the displayed list.
- To disable it, press ```Shift + N``` and Save the file and exit. (Refer the image below)
# Insert image here

### To make KVM a inbuilt feature from a module feature
- Navigate to the <b>Virtualization</b>
- Find <b>Kernel-based Virtual Machine (KVM) support</b>
- To make it a built-in feature, press ```Shift + Y```. (Refer the image below)
- Save the file and exit.
# Insert image here

