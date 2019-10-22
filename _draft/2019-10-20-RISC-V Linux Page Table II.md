---
layout: post
title: RISC-V Linux Page Table II
categories: [RISC-V, Linux, Page Table]
description: Introduction to RISC-V Linux Page Table
keywords: RISC-V, Page Table
---

## RISC-V Linux Page Table II

Generally, Current Linux kernel on different architectures has different types of page table for different usage. For example, AArch64 has three types of pg_dirs, [`idmap_pg_dir`](https://elixir.bootlin.com/linux/v4.14.68/source/arch/arm64/kernel/vmlinux.lds.S#L219), [`swapper_pg_dir`](https://elixir.bootlin.com/linux/v4.14.68/source/arch/arm64/kernel/vmlinux.lds.S#L221) and [`tramp_pg_dir`](https://elixir.bootlin.com/linux/v4.14.68/source/arch/arm64/kernel/vmlinux.lds.S#L230)[^note1]. like AArch64, RISC-V also have three pg_dirs, [`early_pg_dir`](https://elixir.bootlin.com/linux/v5.3.4/source/arch/riscv/mm/init.c#L148) for fixmap, [`swapper_pg_dir`](https://elixir.bootlin.com/linux/v5.3.4/source/arch/riscv/mm/init.c#L141) for kernel page table and [`trampoline_pg_dir`](https://elixir.bootlin.com/linux/v5.3.4/source/arch/riscv/mm/init.c#L142) for meltdown[^note2] fix trampoline. Here my Linux version is 5.3.4



### Setup Virtual memory

RISC-V Linux will setup virtual memory in head.S, line 91, the code is as follows:

```asm
/* Initialize page tables and relocate to virtual addresses */
	la sp, init_thread_union + THREAD_SIZE
	mv a0, s1
	call setup_vm
	la a0, early_pg_dir
	call relocate
```

Function setup_vm is called with an argument which indicates the DTB physical address. Funtion [`setup_vm`](https://elixir.bootlin.com/linux/v5.3.4/source/arch/riscv/mm/init.c#L336) is defined in arch/riscv/mm/init.c.

```c
asmlinkage void __init setup_vm(uintptr_t dtb_pa)
{
	uintptr_t va, end_va;
	uintptr_t load_pa = (uintptr_t)(&_start);
	uintptr_t load_sz = (uintptr_t)(&_end) - load_pa;
	uintptr_t map_size = best_map_size(load_pa, MAX_EARLY_MAPPING_SIZE);

	va_pa_offset = PAGE_OFFSET - load_pa;
	pfn_base = PFN_DOWN(load_pa);

	/*
	 * Enforce boot alignment requirements of RV32 and
	 * RV64 by only allowing PMD or PGD mappings.
	 */
	BUG_ON(map_size == PAGE_SIZE);

	/* Sanity check alignment and size */
	BUG_ON((PAGE_OFFSET % PGDIR_SIZE) != 0);
	BUG_ON((load_pa % map_size) != 0);
	BUG_ON(load_sz > MAX_EARLY_MAPPING_SIZE);

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

	/*
	 * Setup early PGD covering entire kernel which will allows
	 * us to reach paging_init(). We map all memory banks later
	 * in setup_vm_final() below.
	 */
	end_va = PAGE_OFFSET + load_sz;
	for (va = PAGE_OFFSET; va < end_va; va += map_size)
		create_pgd_mapping(early_pg_dir, va,
				   load_pa + (va - PAGE_OFFSET),
				   map_size, PAGE_KERNEL_EXEC);

	/* Create fixed mapping for early FDT parsing */
	end_va = __fix_to_virt(FIX_FDT) + FIX_FDT_SIZE;
	for (va = __fix_to_virt(FIX_FDT); va < end_va; va += PAGE_SIZE)
		create_pte_mapping(fixmap_pte, va,
				   dtb_pa + (va - __fix_to_virt(FIX_FDT)),
				   PAGE_SIZE, PAGE_KERNEL);

	/* Save pointer to DTB for early FDT parsing */
	dtb_early_va = (void *)fix_to_virt(FIX_FDT) + (dtb_pa & ~PAGE_MASK);
}
```








[^note1]: https://wenboshen.org/posts/2018-09-09-page-table.html#dump-raw-page-tables
[^note2]: https://meltdownattack.com/meltdown.pdf

