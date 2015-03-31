cimport defs

cdef extern from "t.h":
	int rnum
	void start(int n)
	int *end()

def pystart(int n):
	defs.start(n)

def pyend():
	defs.end()
