class Sim {

	int rnum, fn, t;

	public:
		Sim(int n);
		void greet(int n);
		void bye(void);

};

Sim::Sim(int n)
{
	rnum = 31415;
	fn = n;
}

void Sim::greet(int n)
{
	int i;
	t = 0;
	for (i=0;i<n;i++) {
		t = t+rnum;
	}
}

void Sim::bye(void)
{
	int a = 0;
	int calcn = t/rnum;
	if (calcn == fn) {
		a = 1;
	}
}
