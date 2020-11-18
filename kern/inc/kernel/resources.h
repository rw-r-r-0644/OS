#pragma once

typedef enum resource_type resource_type;
typedef struct resource resource;

enum resource_type
{
	IORESOURCE_MEM,
};

struct resource
{
	unsigned long type;
	unsigned long start;
	unsigned long n;
	const char *name;
	void *mapping;
	resource *next;
};

resource *
iomemrequest(unsigned long start, unsigned long n, const char *name);

void
iorelease(resource *r);

void *
ioremap(resource *r);

void
iounmap(resource *r);
