cdef extern from "ar.h":
	char addr[4]
	void pop()
	char getElement(int n)
	int getAll()
	void printAll()
