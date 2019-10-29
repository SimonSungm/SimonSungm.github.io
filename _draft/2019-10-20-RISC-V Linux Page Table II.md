---
layout: post
title: RISC-V Linux Page Table II
categories: [RISC-V, Linux, Page Table]
description: Introduction to RISC-V Linux Page Table
keywords: RISC-V, Page Table
---

## RISC-V Linux Page Table II

Generally, Current Linux kernel on different architectures has different types of page table for different usage. For example, AArch64 has three types of pg_dirs, [`idmap_pg_dir`](https://elixir.bootlin.com/linux/v4.14.68/source/arch/arm64/kernel/vmlinux.lds.S#L219), [`swapper_pg_dir`](https://elixir.bootlin.com/linux/v4.14.68/source/arch/arm64/kernel/vmlinux.lds.S#L221) and [`tramp_pg_dir`](https://elixir.bootlin.com/linux/v4.14.68/source/arch/arm64/kernel/vmlinux.lds.S#L230)[^note1]. like AArch64, RISC-V also have three pg_dirs, [`early_pg_dir`](https://elixir.bootlin.com/linux/v5.3.4/source/arch/riscv/mm/init.c#L148) for fixmap, [`swapper_pg_dir`](https://elixir.bootlin.com/linux/v5.3.4/source/arch/riscv/mm/init.c#L141) for kernel page table and [`trampoline_pg_dir`](https://elixir.bootlin.com/linux/v5.3.4/source/arch/riscv/mm/init.c#L142) for meltdown[^note2] fix trampoline. Here my Linux version is 5.3.7 and we focus on 64-bit RISC-V archtecture.



### Setup Virtual memory

RISC-V Linux will setup virtual memory in [arch/riscv/kernel/head.S](), line 91, the code is as follows:

```asm
arch/riscv/kernel/head.S

...
/* Initialize page tables and relocate to virtual addresses */
	la sp, init_thread_union + THREAD_SIZE
	mv a0, s1
	call setup_vm
	la a0, early_pg_dir
	call relocate
...
```

Function [setup_vm]() is called with an argument which indicates the DTB physical address. Funtion [`setup_vm`](https://elixir.bootlin.com/linux/v5.3.4/source/arch/riscv/mm/init.c#L336) is defined in [arch/riscv/mm/init.c]().



```c
arch/riscv/mm/init.c

asmlinkage void __init setup_vm(uintptr_t dtb_pa)
{
...
	/* Setup early PGD for fixmap */
	create_pgd_mapping(early_pg_dir, FIXADDR_START,
			   (uintptr_t)fixmap_pgd_next, PGDIR_SIZE, PAGE_TABLE);

#ifndef __PAGETABLE_PMD_FOLDED
	/* Setup fixmap PMD */
	create_pmd_mapping(fixmap_pmd, FIXADDR_START,
			   (uintptr_t)fixmap_pte, PMD_SIZE, PAGE_TABLE);
	/* Setup trampoline PGD and PMD */
	create_pgd_mapping(trampoline_pg_dir, PAGE_OFFSET,
			   (uintptr_t)trampoline_pmd, PGDIR_SIZE, PAGE_TABLE);
	create_pmd_mapping(trampoline_pmd, PAGE_OFFSET,
			   load_pa, PMD_SIZE, PAGE_KERNEL_EXEC);
#else
	/* Setup trampoline PGD */
	create_pgd_mapping(trampoline_pg_dir, PAGE_OFFSET,
			   load_pa, PGDIR_SIZE, PAGE_KERNEL_EXEC);
#endif
...
}
```

In function [setup_vm](), Linux kernel create two pgds, early_pg_dir and trampoline_pg_dir. Early_pg_dir maps virtual address starting from [FIXADDR_START]() while trampoline_pg_dir maps virtual address starting PAGE_OFFSET. Both of these two pg_dirs map an address space of PGDIR_SIZE (1GB here). There is a macro [__PAGETABLE_PMD_FOLDED](), which is means there is only a two-level page table will be defined only in Sv32 implementation.  



```c
arch/riscv/mm/init.c

static void __init create_pgd_mapping(pgd_t *pgdp,
				      uintptr_t va, phys_addr_t pa,
				      phys_addr_t sz, pgprot_t prot)
{
	pgd_next_t *nextp;
	phys_addr_t next_phys;
	uintptr_t pgd_index = pgd_index(va);

	if (sz == PGDIR_SIZE) {
		if (pgd_val(pgdp[pgd_index]) == 0)
			pgdp[pgd_index] = pfn_pgd(PFN_DOWN(pa), prot);
		return;
	}

	if (pgd_val(pgdp[pgd_index]) == 0) {
		next_phys = alloc_pgd_next(va);
		pgdp[pgd_index] = pfn_pgd(PFN_DOWN(next_phys), PAGE_TABLE);
		nextp = get_pgd_next_virt(next_phys);
		memset(nextp, 0, PAGE_SIZE);
	} else {
		next_phys = PFN_PHYS(_pgd_pfn(pgdp[pgd_index]));
		nextp = get_pgd_next_virt(next_phys);
	}

	create_pgd_next_mapping(nextp, va, pa, sz, prot);
}
```

Function [create_pgd_mapping]() maps virtual address to physical address as well as creates corresponding page table entry. You may notice that when the mapping size equals to [PGDIR_SIZE](), it will only map one page table entry. Recall Sv39 implementation, with a three-level page table, it is enough to use one pgd entry since it has 30 bits offset that maps up to 1GB address space. 



```c
arch/riscv/mm/init.c

asmlinkage void __init setup_vm(uintptr_t dtb_pa)
{	
	...
	end_va = PAGE_OFFSET + load_sz;
	for (va = PAGE_OFFSET; va < end_va; va += map_size)
		create_pgd_mapping(early_pg_dir, va,
				   load_pa + (va - PAGE_OFFSET),
				   map_size, PAGE_KERNEL_EXEC);
    ...
}
```

Then we map the whole kernel from [load_pa]() to [PAGE_OFFSET](), which is the start virtual address of the kernel. Here the [map_size]() equal to [PGDIR_SIZE]() in Rv39. Note, now Linux kernel still read memory using physical address.



```asm
arch/riscv/kernel/head.S

...
	/* Initialize page tables and relocate to virtual addresses */
	la sp, init_thread_union + THREAD_SIZE
	mv a0, s1
	call setup_vm
	la a0, early_pg_dir
	call relocate

	/* Restore C environment */
	la tp, init_task
	sw zero, TASK_TI_CPU(tp)
	la sp, init_thread_union + THREAD_SIZE

	/* Start the kernel */
	call parse_dtb
	tail start_kernel
...
```

Back to head.S, after we setup early_pg_dir, Linux kernel then relocates itself, changing the original physical address into corresonding virtual address. In relocate, Linux kernel setup virtual memory environment by set the ***satp*** register and using virtual address instead of physical address from then on. At last, it parses the Linux device tree blob and calls [start_kernel]().




[^note1]: https://wenboshen.org/posts/2018-09-09-page-table.html#dump-raw-page-tables
[^note2]: https://meltdownattack.com/meltdown.pdf

