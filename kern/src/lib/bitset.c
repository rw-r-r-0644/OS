#include <lib/bitset.h>
#include <lib/stdlib.h>

void
bitset_clrrange(bitset_t *bts, unsigned long start, unsigned long end)
{
	unsigned long current = start;

	while ((BITSET_BIT(current) != 0) && (current < end))
		bitset_clr(bts, current++);

	while ((end - current) > BITSET_IBITS)
	{
		bts->btmem[BITSET_IDX(current)] = 0ul;
		current += BITSET_IBITS;
	}

	while (current < end)
		bitset_clr(bts, current++);
}

void
bitset_setrange(bitset_t *bts, unsigned long start, unsigned long end)
{
	unsigned long current = start;

	while ((BITSET_BIT(current) != 0) && (current < end))
		bitset_set(bts, current++);

	while ((end - current) > BITSET_IBITS)
	{
		bts->btmem[BITSET_IDX(current)] = ~0ul;
		current += BITSET_IBITS;
	}

	while (current < end)
		bitset_set(bts, current++);
}

unsigned long
bitset_findset(bitset_t *bts, unsigned long *outindex)
{
	unsigned long btidx = 0, btelm;

	while ((btidx < bts->btsize) && !bts->btmem[btidx])
		btidx++;
	if (btidx == bts->btsize)
		return 0;

	btelm = __builtin_ctz(bts->btmem[btidx]);
	bts->btmem[btidx] ^= 1 << btelm;

	*outindex = (btidx * BITSET_IBITS) + btelm;
	return 1;
}

unsigned long
bitset_findsetrange(bitset_t *bts, unsigned long count, unsigned long *outindex)
{
	unsigned long btidx = 0, btelm;

	while ((btidx < bts->btsize) && !bts->btmem[btidx])
		btidx++;
	if (btidx == bts->btsize)
		return 0;

	btelm = __builtin_ctz(bts->btmem[btidx]);
	bts->btmem[btidx] ^= 1 << btelm;

	*outindex = (btidx * BITSET_IBITS) + btelm;
	return 1;
}

bitset_t *
bitset_init(unsigned long size, unsigned long init)
{
	unsigned long btlast, btsize, i;
	bitset_t *bts;

	btlast = size & (BITSET_IBITS - 1);
	btsize = size / BITSET_IBITS + !!btlast;

	bts = malloc(sizeof(bitset_t) + BITSET_ISIZE * btsize);
	if (!bts)
		return NULL;
	bts->size = size;
	bts->btsize = size / BITSET_IBITS;

	if (init)
		bitset_setrange(bts, 0, size);
	else
		bitset_clrrange(bts, 0, size);

	return bts;
}
