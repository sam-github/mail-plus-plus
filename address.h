//
// Mail address construction and parsing
//

#ifndef M_ADDRESS_H
#define M_ADDRESS_H

#include <rope>
#include <vector>

class MAddrSpec
{
private:
	crope	text_;
	crope	local_part_;
	crope	domain_;

public:
	MAddrSpec()
	{
	}
	// need error checking on validity of input characters, may need
	// quotes, neither option is optional, perhaps invalid input should
	// result in a Null addr-spec.
	MAddrSpec(const crope& local_part, const crope& domain) :
		local_part_ (local_part), domain_ (domain)
	{
		Text();
	}

	const crope& Text()
	{
		text_.clear();

		text_ += local_part_;
		text_ += "@";
		text_ += domain_;

		return text_;
	}

	const crope& Text() const
	{
		return text_;
	}

	const crope& LocalPart() const
	{
		return local_part_;
	}
	const crope& Domain() const
	{
		return domain_;
	}
};

class MMailBox
{
private:
	typedef const crope& Rope;

	crope		local_part_;
	crope		domain_;
	crope		display_name_;

	crope		text_;

	Rope Build()
	{
		text_.clear();

		if(!display_name_.empty())
		{
			text_ += display_name_;
			text_ += " <";
		}
		text_ += local_part_;

		// This isn't RFC compliant, but is a valid address on most
		// systems.
		if(!domain_.empty())
		{
			text_ += '@';
			text_ += domain_;
		}

		if(!display_name_.empty())
		{
			text_ += ">";
		}
		return text_;
	}
public:
	MMailBox()
	{
	}
	MMailBox(Rope local_part, Rope domain, Rope display_name = "") :
		local_part_ (local_part), domain_ (domain), display_name_ (display_name)
	{
		Build();
	}
	Rope LocalPart() const
	{
		return local_part_;
	}
	Rope Domain() const
	{
		return domain_;
	}
	Rope DisplayName() const
	{
		return display_name_;
	}
	Rope Text() const
	{
		return text_;
	}
};

typedef vector<MMailBox>	MMailBoxList;

template <class T>
crope MMailBoxListText(T b, T e)
{
	crope text;

	while(b != e)
	{
		MMailBox mbox = *b;

		if(mbox.Text().empty())
			continue;

		if(!text.empty())
			text += ", ";

		text += mbox.Text();

		++b;
	}
	return text;
}

crope MMailBoxListText(const MMailBoxList& list)
{
	return MMailBoxListText(list.begin(), list.end());
}

#endif

