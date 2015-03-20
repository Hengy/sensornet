#include <stdio.h>

#include "ar.h"

void pop(void)
{
	addr[0] = 0x55;
	addr[1] = 0x77;
	addr[2] = 0x33;
	addr[3] = '\0';
}

char getElement(int n)
{
	return addr[n-1];
}

void putElement(int n, char c)
{
	addr[n] = c;
}

void putAll(int n, char c[])
{
	char d[n];
	int i;
	for (i=0;i<n;i++) {
		d[i] = c[i];
	}
	memcpy(addr, d, n);
}

char *getAll(void)
{
	return &addr[0];
}

void printAll(void)
{
	puts(addr[0]);
	puts("\n");
}
