//
// Mail address construction and parsing
//

#ifndef M_ADDRESS_H
#define M_ADDRESS_H

#include <rope>

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
	crope		text_;
	crope		phrase_;
	MAddrSpec	addr_spec_;

public:
	MMailBox(const MAddrSpec& addr_spec, const crope& phrase = "") :
		addr_spec_ (addr_spec), phrase_ (phrase)
	{
		Text();
	}

	MMailBox(const crope& addr_spec, const crope& domain, const crope& phrase = "") :
		addr_spec_ (addr_spec, domain), phrase_ (phrase)
	{
	}
	const MAddrSpec& AddrSpec() const
	{
		return addr_spec_;
	}
	const crope& Phrase() const
	{
		return phrase_;
	}
	const crope& Text() const
	{
		return text_;
	}
	const crope& Text()
	{
		text_ = addr_spec_.Text();
		if(!phrase_.empty())
		{
			text_ += " (";
			text_ += phrase_;
			text_ += ")";
		}
		return text_;
	}
};

#endif

