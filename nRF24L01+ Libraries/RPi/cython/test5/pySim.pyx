cdef extern from "sim.cpp":
	cdef cppclass Sim:
		Sim(int n)
		void greet(int n)
		void bye()

cdef class pySim:
	cdef Sim* P
	def __cinit__(self, int n):
		self.p = new Sim(4)

	def __dealloc__(self):
		del self.p

	def greet(self, int n):
		self.p.greet(n)

	def bye(self):
		self.p.bye()
