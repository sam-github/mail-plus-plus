//
// Mail address construction and parsing
//

#ifndef M_ADDRESS_H
#define M_ADDRESS_H

#include <rope>
#include <vector>

class MMailBox;

class MAddrSpec
{
private:
	typedef const crope& Rope;

	crope local_part_;
	crope domain_;

	crope text_;

	void Build();

public:
	MAddrSpec();
	MAddrSpec(const MMailBox& mailbox);
	MAddrSpec(Rope local_part, Rope domain);

	Rope LocalPart() const;
	Rope Domain() const;

	Rope Text() const;
};

class MMailBox
{
private:
	typedef const crope& Rope;

	MAddrSpec	addr_spec_;
	crope		display_name_;

	crope		text_;

	void Build();

public:
	MMailBox();
	MMailBox(const MAddrSpec& addr_spec, Rope display_name = "");
	MMailBox(Rope local_part, Rope domain, Rope display_name = "");

	const MAddrSpec& AddrSpec() const;
	Rope LocalPart() const;
	Rope Domain() const;
	Rope DisplayName() const;

	Rope Text() const;
};

/**
* A list of mailbox elements.
*/
typedef vector<MMailBox> MMailBoxList;
/**
* A list of addr-spec elements.
*/
typedef vector<MAddrSpec> MAddrSpecList;

crope MAddressListToText(const MAddrSpecList& addrspecs);
crope MAddressListToText(const MMailBoxList& mailboxes);

// Do I want to provide a quick-and-dirty validity test
// for addresses?
int MCheckAddress(const MAddrSpec& addr_spec);
int MCheckAddress(const MMailBox& mailbox);

/**
* The syntax of local-part is restrictive; if any special characters
* are used they need to be quoted. This function takes the simple
* approach of determining if there are any special characters in the
* local-part, and if so quotes the entire text. If it is already
* mail safe then no transformation is applied.
*
* Note that the .Text() of the address's are all correctly quoted,
* there is no need to call this on them.
*
* @arg local_part is a local-part to be quoted so as to be mail safe.
* @return the local_part as mail safe text.
*/
crope MQuoteLocalPart(const crope& local_part);

#endif

