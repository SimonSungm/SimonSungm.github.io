---
layout: post
title:  "Cross Compile Gcc 9.1 for ARM64 on Ubuntu 18.04"
categories: [ARM64, Cross Compiler]
description: This is a record of how I cross compile Gcc 9.1 on Ubuntu 18.04.
keywords: ARM64, Cross Compiler
---

This is a record of how I cross compile Gcc 9.1 on Ubuntu 18.04.

## Environment
### 1. Host Environment 

    host : Ubuntu 18.04
    GCC version : 9.1.0(you may use defalut compiler of your Ubuntu)
    Binutils version : 2.3
    Glibc version : 2.27
    Bison version : 3.0.4
    Flex version : 2.6.4
    GMP version : 6.1.2
    MPC version : 1.1.0
    MPFR version : 4.0.1
    Texinfo version : 6.5.0
    CLooG version : 0.18.4
    ISL version : 0.19

### 2. Target

    Source dir : /media/cliff/Work2/testKernel/src/
    Target dir : $HOME/opt/cross
    Gcc version : 9.1
    Binutils version : 2.32
    Linux kernel version : 5.0.1
    Glibc version : 2.30



## Step

### 1. Download Source
You can download source files from following sites.

Binutils : [Binutils website](https://www.gnu.org/software/binutils/), [the GNU main mirror](https://ftp.gnu.org/gnu/binutils/).

GCC : [GCC website](https://www.gnu.org/software/gcc/), [the GNU main mirror](https://ftp.gnu.org/gnu/gcc/).

Linux : [Index of /pub/linux/kernel/](https://mirrors.edge.kernel.org/pub/linux/kernel/)

Glibc : [Index of /gnu/glibc/](http://mirrors.kernel.org/gnu/glibc/)
    


```shell
$ cd /media/cliff/Work2/testKernel/src/
$ ls 
    binutils-2.32   gcc-9.1   glibc-2.30    linux-5.0.1
$ touch asan_linux-cc.patch
```
Copy following code to the patch.

```c
--- orig/asan_linux.cc    2019-07-11 21:18:56.000000000 +0100
+++ mod/asan_linux.cc	2019-07-11 16:31:42.000000000 +0100
@@ -75,6 +75,10 @@
 asan_rt_version_t  __asan_rt_version;
 }

+#ifndef PATH_MAX
+#define PATH_MAX 4096
+#endif
+
 namespace __asan {

 void InitializePlatformInterceptors() {}
```
```shell
$ patch -b gcc-9.1/libsanitizer/asan/asan_linux.cc asan_linux-cc.patch
```
You can find the backed up file as gcc-9.1/libsanitizer/asan/asan_linux.cc.orig


### 2. Set Environment Values
```shell
export PREFIX="$HOME/opt/cross"
export TARGET=aarch64-linux
export PATH="$PREFIX/bin:$PATH"
```

### 3. Build Binutils
```shell
cd /media/cliff/Work2/testKernel/src/
mkdir build-binutils
cd build-binutils
../binutils-2.32/configure --target=$TARGET --prefix=$PREFIX --with-sysroot --disable-multilib
make -j12
make install
cd ..
```

### 4. Linux Kernel Headers
```shell
cd linux-5.0.1
make ARCH=arm64 INSTALL_HDR_PATH=$PREFIX/$TARGET headers_install
cd ..
```

### 5. Build Gcc
```shell
mkdir -p build-gcc
cd build-gcc
../gcc-9.1/configure --prefix=$PREFIX --target=$TARGET --enable-languages=c,c++ --disable-multilib
make -j12 all-gcc
make install-gcc
cd ..
```

### 6. Build Glibc
```shell
mkdir -p build-glibc
cd build-glibc
../glibc-2.30/configure --prefix=$PREFIX/$TARGET --build=$MACHTYPE --host=aarch64-linux --target=aarch64-linux --with-headers=$PREFIX/$TARGET/include --disable-multilib libc_cv_forced_unwind=yes
make install-bootstrap-headers=yes install-headers
make -j12 csu/subdir_lib
install csu/crt1.o csu/crti.o csu/crtn.o $PREFIX/$TARGET/lib
aarch64-linux-gcc -nostdlib -nostartfiles -shared -x c /dev/null -o $PREFIX/$TARGET/lib/libc.so
touch $PREFIX/$TARGET/include/gnu/stubs.h
cd ..
```

### 7. Compiler Support Library
```shell
cd build-gcc
make -j12 all-target-libgcc
make install-target-libgcc
cd ..
```

### 8. Standard C Library
```shell
cd build-glibc
make -j12
make install
cd ..
```

### 9.Standard C++ Library
```shell
cd build-gcc
make -j12
make install
cd ..
```

## Reference
[1] [https://wiki.osdev.org/GCC_Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler)

[2] [https://preshing.com/20141119/how-to-build-a-gcc-cross-compiler/](https://preshing.com/20141119/how-to-build-a-gcc-cross-compiler/)

[3] [http://docs.slackware.com/howtos:hardware:arm:gcc-9.x_aarch64_cross-compiler](http://docs.slackware.com/howtos:hardware:arm:gcc-9.x_aarch64_cross-compiler)
