#pragma once
#define BITSET_ISIZE	(sizeof(unsigned long))
#define BITSET_IBITS	(BITSET_ISIZE << 3)
#define BITSET_IDX(i)	(i / BITSET_IBITS)
#define BITSET_BIT(i)	(i & (BITSET_IBITS - 1))

typedef struct
{
	unsigned long size;
	unsigned long btsize;
	unsigned long btmem[];
} bitset_t;

static inline void
bitset_clr(bitset_t *bts, unsigned long index)
{
	bts->btmem[BITSET_IDX(index)] &=
		~(1 << BITSET_BIT(index));
}

static inline void
bitset_set(bitset_t *bts, unsigned long index)
{
	bts->btmem[BITSET_IDX(index)] |=
		1 << BITSET_BIT(index);
}

static inline void
bitset_toggle(bitset_t *bts, unsigned long index)
{
	bts->btmem[BITSET_IDX(index)] ^=
		1 << BITSET_BIT(index);
}

static inline unsigned long
bitset_test(bitset_t *bts, unsigned long index)
{
	return (bts->btmem[BITSET_IDX(index)] >>
		BITSET_BIT(index)) & 1;
}

void
bitset_clrrange(bitset_t *bts, unsigned long start, unsigned long end);

void
bitset_setrange(bitset_t *bts, unsigned long start, unsigned long end);

unsigned long
bitset_findset(bitset_t *bts, unsigned long *outindex);

unsigned long
bitset_findsetrange(bitset_t *bts, unsigned long count, unsigned long *outindex);

bitset_t *
bitset_init(unsigned long size, unsigned long init);
