#pragma once

#define _packed				__attribute__((packed))
#define _noreturn			__attribute__((noreturn))
#define _aligned(a)			__attribute__((aligned(a)))

#define _align2_up(n, a)		((n + (a - 1)) & -a)
#define _align2_down(n, a)	(n & -a)

