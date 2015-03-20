cdef extern from "hello.h":
	int greet(int n)

def greet(int n):
	print n
