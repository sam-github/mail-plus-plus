//
// this code segvs
//

#include <rope>

class Thing
{
private:
	crope name_;
	crope text_;

public:
	Thing(const crope& name = "")
//		: name_(name) , text_(name)
	{
		name_ = name;
	}
};

void foo(const crope& in = "")
{
	crope a = in;
	crope b = in;
}

int main()
{
//	foo();

	Thing thing;

	return 0;
}

