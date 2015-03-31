cimport defns

cimport cython

from libc.stdlib cimport malloc, free

def pypop():
	defns.pop()

def pygetElement(int n):
	print defns.getElement(n)

def pyputElement(int n, char c):
	defns.putElement(n, c)

def pyputAll(int n, c):
	cdef int *ptr
	ptr = <int *>malloc(n*cython.sizeof(int))
	if ptr is NULL:
		raise MemoryError()
	for i in xrange(n):
		ptr[i] = c[i]
	defns.putAll(n, ptr)
	free(ptr)

def pygetAll():
	cdef char* cnrf = defns.getAll()
	cdef bytes nrf = cnrf
	print nrf

def pyprintAll():
	defns.printAll()
