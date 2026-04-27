/**
 * memory.h — Virtual RAM & Custom Memory Allocator Header
 * ========================================================
 * 64 KB static virtual address space with bump + free-list allocator.
 * No malloc() / free() from <stdlib.h>.
 */

#ifndef MEMORY_H
#define MEMORY_H

/* ── Allocator Interface ────────────────────────────────────────────────  */
void *my_alloc(int size);       /* allocate 'size' bytes from virtual RAM */
void  my_dealloc(void *ptr);    /* return block to free-list              */
void  mem_reset(void);          /* wipe virtual RAM (for game restart)    */

/* ── Diagnostics ──────────────────────────────────────────────────────── */
int   mem_alloc_count(void);    /* number of live allocations             */
int   mem_bytes_used(void);     /* bytes consumed from virtual RAM        */
int   mem_bytes_total(void);    /* total virtual RAM size (65536)         */
int   mem_total_allocated(void);/* cumulative bytes ever allocated        */

#endif /* MEMORY_H */
