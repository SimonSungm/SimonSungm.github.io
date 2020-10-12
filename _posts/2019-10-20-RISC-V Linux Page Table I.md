---
layout: post
title: RISC-V Linux Page Table I
categories: [RISC-V, Linux, Page Table]
description: Introduction to RISC-V Linux Page Table
keywords: RISC-V, Page Table
---

The RISC-V page table formats can be found in [`RISC-V Privileged ISA Specification`](https://riscv.org/specifications/privileged-isa/). Current RISC-V (**Document Version 20190608-Priv-MSU-Ratified**) supports three types of virtual memory, Sv32 for RV32, Sv39 and Sv48 for RV64, which supports 32-bit, 39-bit and 48-bit virtual address space respectively.



## Sv32 Implementation

![](/images/posts/riscv-page-table/Sv32.png)

The page table format of Sv32 is as above. The page size of Sv32 is 4 KiB, which is popular page size. Thus,  It has a 12-bit long page offset. With each page table entry(**PTE**) being 4 bytes, each page table contains 1024 page table entries. The higher 20 bits of virtual address (virtual page number) is divide into two parts, VPN[0] and VPN[1], 10 bits each to support a two-level page table. 

The 20 bits virtual page number is translated into a 22-bit physical page number via a two-level page table. In addition, Sv32 also supports 4 MiB megapages.

The lower 10 bits of a page table entry encodes the ***protection bits*** of this page. The meaning of each bit is as follows:

-  V: This bit determines if the remaining bits are valid. When V = 0, any virtual address translation operation that traverses to this page table entry will result in a page fault.
- R: This bit determines whether this page is readable, readable when R = 1.
- W: This bit determines whether this page is writable, writable when W = 1.
- X: This bit determines whether this page is executable, executable when X = 1.
- U: This bit determines whether this page belongs to the user. If U = 0, U mode cannot access this page. If U = 1, U mode can access this page and S mode can access this page only when ***SUM bit*** in the sstatus register is set. And this bit must be cleared for non-leaf PTEs.
- G: This bit indicates whether this mapping is valid for all virtual address spaces, and the hardware can use this information to improve the performance of address translation. Generally, this bit is only used for pages belonging to the operating system.
- A: Accessed bit, must be cleared for non-leaf PTEs.
- D: Dirty bit, must be cleared for non-leaf PTEs.
- RSW: These two bits are reserved for the operating system and will be ignored by hardware.

Besides, different combinations of R, W and X bits also have special meanings.

![](/images/posts/riscv-page-table/PTE bits.png)



## Sv39 Implementation

![](/images/posts/riscv-page-table/Sv39-1.png)

![](/images/posts/riscv-page-table/Sv39-2.png)

Sv39 implementation support 39-bit virtual address space, divided into 4 KiB pages. As the PTE of Sv39 has 8 bytes with the highest 10 bits reserved and must be set to 0, there are only 512 rather than 1024 PTEs on one page.

Sv39 uses a three-level page table that higher bits of virtual address is divided into three fields, 9 bits each. And bits 63-39 of the virtual are equal to bit 38 to help the operating system to distinguish user and supervisor address region quickly. Sv39 is able to support 2 MiB megapages and 1GiB gigapages.

And the other design of Sv39 follows the overall scheme of Sv32. 



## Sv48 Implementation

![](/images/posts/riscv-page-table/Sv48.png)

Sv48 supports a 4-level page table and 512 GiB terapages besides 4 KiB pages, 2 MiB megapages, and 1 GiB gigapages. Sv48 implementation is much similar to Sv39 implementation, so I will not introduce it in detail.



## Virtual Address Translation

Virtual address is translated to physical address level by level. I briefly introduce this translation, the whole translation algorithm can be found in privileged ISA specification.

1. Get the root page table number from ***satp*** register's PPN field.
2. Find a PTE use the first VPN field of virtual address, and it may raise ***an access exception***.
3. Determined whether it's a valid PTE according to the lowest four bits. If not, raise ***a page-fault exception***. Otherwise, if the XWR bits are all 0, the PPN field is the physical page number of the next level page table. Then it walks the next level page table using the next VPN field and goes to step 2 again until a leaf PTE is founded. If there is no next VPN field then it will raise a **page-fault exception**. 
4. Check whether the memory access is allowed according to PTE bits, current privilege mode and some fields of ***mstatus*** register. If not allowed, raise a **page-fault exception**. 
5. If it is not a 4 KiB page, which means there are remaining VPN fields unused before, then check whether corresponding PPN fileds are all 0. If not, raise a **page-fault exception**.  For example, if VPN[0] filed has not been used before, then PPN[0] must be all 0.
6. Do the remaining work according to A bit, D bit and access type, here may raise **an access exception** (If this access violates a PMA or PMP check) or **a page-fault exception.**
7. Get the physical address according to current PTE, current page table level, remaining unused VPN field and the page offset of virtual address.



## Reference

[1] [RISC-V Privileged ISA Specification](https://riscv.org/specifications/privileged-isa/)

