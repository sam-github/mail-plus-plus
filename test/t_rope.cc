//
// crope test code
//

#include <stdio.h>
#include <assert.h>

#pragma warning 4 9

#include <mail++/rope.h>

typedef crope::const_iterator Ptr;

void contents(const crope& cr)
{
	printf("cr '%s': e - b -> %d\n", cr.c_str(), cr.end() - cr.begin());
}

void t_mutable()
{
	crope cr; contents(cr);

	cr = ""; contents(cr);

	cr.clear(); contents(cr);

	assert(cr.mutable_end() == cr.mutable_end());
}

void t_inc()
{
	const crope r = "hello";

	crope::const_iterator p = r.begin();

	assert(*(p + 4) == 'o');
	assert((p + 5) == r.end());
	assert((p + 6) == r.end());
	assert((p + 20) == r.end());
}

void t_permute()
{
}

int main()
{
	t_mutable();
	t_inc();
	t_permute();

	return 0;
}

