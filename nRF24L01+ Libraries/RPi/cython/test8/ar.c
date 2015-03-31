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

int getAll(void)
{
	return &addr[0];
}

void printAll(void)
{
	puts(addr[0]);
}
