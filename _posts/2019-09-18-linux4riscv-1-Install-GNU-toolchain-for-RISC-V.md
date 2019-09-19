---
layout: post
title:  "linux4riscv-1: Install GNU toolchain for RISC-V"
description: Record of how to install GNU toolchain for RISC-V
date:   2019-09-18 15:15:36 +0530
categories: Linux for RISC-V 
---

Recently, my instructor decided to update the project of operating system course. The new project is about porting Linux 0.11 to ARM64 and RISC-V as well as rewriting its interrupt, memory management, and file system, etc. My task is on RISC-V and I have no knowledge about it before, so I am to record what I have done and what I have learned. This is the first blog of this series. However, the source code will not be open-sourced as it will be used in operating system teaching in the future. Anyway, it's a big project and hopefully I can finish it before the final exam.



## Install GNU toolchain for RISC-V

#### Download source code of riscv-gnu-toochain
```shell
$ mkdir ~/Documents/linux4riscv
$ cd ~/Documents/linux4riscv
$ git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
$ cd riscv-gnu-toolchain
```
It may be too slow (50 KB/s) and too many problems(I downloaded it for a day, 我佛了), so I have uploaded it onto [浙大云盘](https://pan.zju.edu.cn/share/ad165bddc1b4cce7dbaa9d089e) (Only ZJU students are allowed).

**If you choose to download prebuilt compiler,  you can skip these three steps and go ahead to step Test our Installation**.

#### Prerequisites

```shell
$ sudo apt-get install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev
```
#### Installation (Newlib)

```shell
$ ./configure --prefix=/home/cliff/opt/riscv
$ make
```

Here we use /home/cliff/opt/riscv as default path of our toolchain. You can modify it as you like. It needs about 8GB spare space and take a long time to compile. So I have upload the binary onto [浙大云盘](https://pan.zju.edu.cn/share/ad165bddc1b4cce7dbaa9d089e) to reduce your workload.

#### Installation (Linux)

**note:** this step is not necessary as we need to write our own operating system rather than just run program on the Linux. You can just skip this step. 

```shell
$ ./configure --prefix=/home/cliff/opt/riscv
$ make linux
```

#### Test our Installation

Well, this step is absolutely not fun since you have to install a series of libraries, and you will get a lot of error when you install these libraries. Unfortunately, since we want to use qemu to test our to be built kernel, this step (it will build qemu for RISC-V) can not be skipped. **But** **you can just stop when you successfully build qemu-riscv64**. Have fun : ).

**Tips :** You can find related library package name you need use **sudo apt-cache search package-name** and then use **sudo apt install package-name**. For example, if you type sudo apt-cache search gcc, then you will get a series of package with a string of gcc in its name.

```shell
$ ./configure --prefix=$RISCV --disable-linux --with-arch=rv64ima # or --with-arch=rv32ima
$ make newlib
$ make report-newlib
```

You can skip the following step is you do not do  step Installation(Linux)

```shell
$ ./configure --prefix=$RISCV
$ make linux
$ make report-linux
```
If you get the following output, congratulations! I was going to say it, but I failed after I successfully built qemu. So forget it, or you can confirm it by yourself. 



## Reference

[1] [RISC-V GNU Compiler Toolchain](https://github.com/riscv/riscv-gnu-toolchain#risc-v-gnu-compiler-toolchain)

