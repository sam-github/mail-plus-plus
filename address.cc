//
// Mail address construction and parsing
//

#include <mail++/address.h>
#include <mail++/parse822.h>
#include <mail++/quote.h>

//
// MAddrSpec
//

MAddrSpec::MAddrSpec()
{
}
MAddrSpec::MAddrSpec(const MMailBox& mailbox)
{
	*this = mailbox.AddrSpec();
}
MAddrSpec::MAddrSpec(Rope local_part, Rope domain) :
	local_part_ (local_part), domain_ (domain)
{
	text_ += MQuoteLocalPart(local_part_);

	// This isn't RFC compliant, but is a valid address on most
	// systems.
	if(!domain_.empty())
	{
		text_ += '@';
		text_ += domain_;
	}
}
MAddrSpec::Rope MAddrSpec::LocalPart() const
{
	return local_part_;
}
MAddrSpec::Rope MAddrSpec::Domain() const
{
	return domain_;
}
MAddrSpec::Rope MAddrSpec::Text() const
{
	return text_;
}

//
// MMailBox
//

void MMailBox::Build()
{
	text_.clear();

	if(!display_name_.empty())
	{
		text_ += display_name_;
		text_ += " <";
	}

	text_ += addr_spec_.Text();

	if(!display_name_.empty())
	{
		text_ += ">";
	}
}
MMailBox::MMailBox()
{
}
MMailBox::MMailBox(const MAddrSpec& addr_spec, Rope display_name) :
	addr_spec_ (addr_spec), display_name_ (display_name)
{
	Build();
}
MMailBox::MMailBox(Rope local_part, Rope domain, Rope display_name) :
	addr_spec_ (local_part, domain), display_name_ (display_name)
{
	Build();
}
const MAddrSpec& MMailBox::AddrSpec() const
{
	return addr_spec_;
}
MMailBox::Rope MMailBox::LocalPart() const
{
	return addr_spec_.LocalPart();
}
MMailBox::Rope MMailBox::Domain() const
{
	return addr_spec_.Domain();
}
MMailBox::Rope MMailBox::DisplayName() const
{
	return display_name_;
}
MMailBox::Rope MMailBox::Text() const
{
	return text_;
}

//
// Helper Functions
//

// maybe should be public?
template <class T>
crope MakeList(const T& list)
{
	crope text;

	T::const_iterator p = list.begin();
	T::const_iterator e = list.end();

	while(p != e)
	{
		if(p->Text().empty())
			continue;

		if(!text.empty())
			text += ", ";

		text += p->Text();

		++p;
	}
	return text;
}
crope MAddressListToText(const MAddrSpecList& list)
{
	return MakeList(list);
}
crope MAddressListToText(const MMailBoxList& list)
{
	return MakeList(list);
}
crope MQuoteLocalPart(const crope& local_part)
{
	crope quoted = local_part;

	MLocalPartQuoter quoter;

	quoter.Quote(quoted);

	return quoted;
}

