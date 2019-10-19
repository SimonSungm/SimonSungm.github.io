---
layout: post
title: QARMA encryption algorithm
categories: [Cryptography, Encryption Algorithm]
description: QARMA encryption algorithm 
keywords: QARMA, Encryption
---

## What is QARMA

QARMA is an encryption algorithm propose by Roberto Avanzi, Qualcomm. The original paper of QARMA encryption algorithm can be found [here](https://eprint.iacr.org/2016/444.pdf). It's a **tweakable block ciphers**[^note2]  which is suitable be implemented by hardware. Different from AES, which is designed as a general-purpose algorithm, QARMA is a lightweight block ciphers motivated by the need to provide an acceptable security level for specific applications at much lower area, latency, or power consumption levels[^note1].  

QARMA supports block size of both n = 64 bits and n = 128 bits, denoted by QARMA-64 and QARMA-128, respectively. The length of encryption key is 2n bits while that of tweak is n bits. QARMA is implemented on ARMv8.3 as the encryption algorithm part of its new Pointer Authentication feature[^note3]. And I will not introduce this new feature in this article.



## Specification of QARMA

In this section, I will introduce how QARMA works in detail.

![QARMA_Structure](/home/cliff/Documents/SimonSungm.github.io/image/QARMA/QARMA_Structure.png)

<center>Figure 1: The Structure of QARMA<sub>r</sub></center>
Figure 1 shows the structure of QARMA Algorithm with encryption/decryption round being assigned r. The meaning of each symbol is as follows:

- **P**: The input n bits plaintext

- **C**: The output ciphertext

- **T**: The input n bits tweak

- **w<sup>0</sup>,w<sup>1</sup>,k<sup>0</sup>,k<sup>1</sup>**: Keys that derived from a input 2n bits master key K via a simple key specialisation

- **c<sub>0</sub>,c<sub>1</sub>,...,c<sub>r-1</sub>**: The round constants

- **h**, **tau**: Permutations. we will use **h** and **tau** to indicate following two symbols respectively.
$$
h,\tau
$$
- **omega**: Linear feedback shift register (LFSR). we will use **omega** to indicate the following symbol
  $$
  \omega
  $$
  
- **S**: A layer of sixteen m bits (we have n = 16m) S-boxes
  
- **M,Q**: MixColumn-like opeations, with Q involutory







## Reference

[^note1]: [The QARMA Block Cipher Family](https://eprint.iacr.org/2016/444.pdf)
[^note2]: https://link.springer.com/content/pdf/10.1007%2F3-540-45708-9_3.pdf
[^note3]: https://www.qualcomm.com/media/documents/files/whitepaper-pointer-authentication-on-armv8-3.pdf