//
// this code segvs
//

#include <rope>

void foo(const crope& in = "")
{
	crope a;
	a = in;
}

int main()
{
	foo();

	return 0;
}

