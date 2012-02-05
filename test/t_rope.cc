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
//	assert((p + 6) == r.end());
//	assert((p + 7) == r.end());
}

void local(crope::iterator p)
{
	cout << "p-1 -> " << *(p - 1) << endl;
	cout << "p   -> " << *(p    ) << endl;
	cout << "p+1 -> " << *(p + 1) << endl;
}

void t_permute()
{
	crope r;

	r += "hello, ";
	r += "world, ";
	r += "aaaaaaaaaaaaaaaaaaaaaaaaaa,  ,,";
	r += ",,  a ,,,  ";

	for(crope::iterator p = r.mutable_begin(); p != r.mutable_end(); ++p)
	{
		if(*p == ',') {
//			local(p);
			p = r.insert(p, '|');
			assert(*p == '|');
			++p;
			assert(*p == ',');
//			local(p);
		}
	}
	cout << "rope '" << r << "'" << endl;
}

int main()
{
//	t_mutable();
//	t_inc();
	t_permute();

	return 0;
}

