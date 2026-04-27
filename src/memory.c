/**
 * memory.c — Virtual RAM & Custom Memory Allocator
 * =================================================
 * Simulates an OS memory subsystem.
 *
 * Architecture:
 *   - A static 64 KB "virtual RAM" array acts as the heap.
 *   - A bump pointer (vram_top) tracks the next free byte.
 *   - A free-list allows reuse of deallocated blocks.
 *   - Allocation metadata (block header) is stored in-band.
 *
 * Block Layout in virtual_ram:
 *   [ BlockHeader (8 bytes) | user data (size bytes) ]
 *
 * NO malloc() / free() from <stdlib.h> is used.
 */

#include "memory.h"

/* ── Virtual RAM Region ───────────────────────────────────────────────── */
#define VRAM_SIZE  65536           /* 64 KB virtual address space */
#define ALIGN      4               /* 4-byte alignment */
#define HDR_MAGIC  0xDEADBEEF     /* sanity sentinel for block headers */

static char         virtual_ram[VRAM_SIZE];   /* the heap */
static int          vram_top  = 0;             /* bump pointer (bytes used) */
static int          alloc_count = 0;           /* live allocations */
static int          total_allocated = 0;       /* cumulative bytes allocated */

/* ── Block Header ─────────────────────────────────────────────────────── */
typedef struct BlockHeader {
    unsigned int magic;            /* HDR_MAGIC sentinel */
    int          size;             /* payload size in bytes */
    int          free;             /* 1 = free, 0 = in use */
    struct BlockHeader *next_free; /* pointer for free-list */
} BlockHeader;

#define HDR_SIZE  sizeof(BlockHeader)

/* ── Free List Head ───────────────────────────────────────────────────── */
static BlockHeader *free_list = 0;   /* head of free-list (NULL initially) */

/* ── Alignment Helper ─────────────────────────────────────────────────── */
static int align_up(int n) {
    int rem = n & (ALIGN - 1);      /* n % ALIGN using bitmask (power of 2) */
    return rem ? n + (ALIGN - rem) : n;
}

/* ── my_alloc ─────────────────────────────────────────────────────────── */
/**
 * my_alloc — allocate 'size' bytes from virtual RAM.
 *
 * Strategy:
 *  1. Search free-list for a block >= size (first-fit).
 *  2. If found, mark it used and return its payload pointer.
 *  3. Otherwise, bump vram_top and carve a new block.
 *  4. Returns 0 (NULL) if virtual RAM is exhausted.
 */
void *my_alloc(int size) {
    if (size <= 0) return 0;

    int aligned_size = align_up(size);

    /* ── Phase 1: Search free list ──────────────────── */
    BlockHeader *prev = 0;
    BlockHeader *cur  = free_list;
    while (cur) {
        if (cur->size >= aligned_size) {
            /* Remove from free list */
            if (prev) prev->next_free = cur->next_free;
            else       free_list      = cur->next_free;
            cur->free       = 0;
            cur->next_free  = 0;
            alloc_count++;
            return (void *)((char *)cur + HDR_SIZE);
        }
        prev = cur;
        cur  = cur->next_free;
    }

    /* ── Phase 2: Bump allocate ──────────────────────── */
    int needed = (int)HDR_SIZE + aligned_size;
    if (vram_top + needed > VRAM_SIZE) return 0;   /* OOM */

    BlockHeader *hdr = (BlockHeader *)(virtual_ram + vram_top);
    hdr->magic      = HDR_MAGIC;
    hdr->size       = aligned_size;
    hdr->free       = 0;
    hdr->next_free  = 0;

    vram_top        += needed;
    alloc_count++;
    total_allocated += aligned_size;

    return (void *)((char *)hdr + HDR_SIZE);
}

/* ── my_dealloc ───────────────────────────────────────────────────────── */
/**
 * my_dealloc — release a previously allocated block.
 * Validates the HDR_MAGIC sentinel to detect double-free or bad pointer.
 * Adds the block to the free list for reuse.
 */
void my_dealloc(void *ptr) {
    if (!ptr) return;

    BlockHeader *hdr = (BlockHeader *)((char *)ptr - HDR_SIZE);

    /* Validate sentinel */
    if (hdr->magic != HDR_MAGIC) return;   /* corrupt/invalid pointer */
    if (hdr->free)               return;   /* already freed */

    hdr->free      = 1;
    hdr->next_free = free_list;
    free_list      = hdr;
    alloc_count--;
}

/* ── Diagnostics ──────────────────────────────────────────────────────── */
int mem_alloc_count(void)   { return alloc_count; }
int mem_bytes_used(void)    { return vram_top; }
int mem_bytes_total(void)   { return VRAM_SIZE; }
int mem_total_allocated(void) { return total_allocated; }

/**
 * mem_reset — wipe the entire virtual RAM (for game restart).
 */
void mem_reset(void) {
    int i;
    for (i = 0; i < VRAM_SIZE; i++) virtual_ram[i] = 0;
    vram_top        = 0;
    alloc_count     = 0;
    total_allocated = 0;
    free_list       = 0;
}
