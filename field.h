//
// An RFC822 header field
//

#ifndef M_FIELD_H
#define M_FIELD_H

#include <mail++/rope.h>

class MField
{
private:
	typedef crope Rope;
	typedef crope::const_iterator Ptr;

	crope	name_;
	crope	value_;
	crope	text_;

public:
	MField();
	MField(const crope& name, const crope& value);
	MField(Ptr& p, const Ptr& e);

	static int Write(Rope& text, const Rope& name, const Rope& value);
	static int Read(Ptr& p, const Ptr& e, Rope& name, Rope& value);

	/**
	* @return the name of the field.
	*/
	const crope& Name() const;
	/**
	* @return the value of the field, unfolded.
	*/
	const crope& Value() const;
	/**
	* @return the complete, folded, text of the field (name and value).
	*/
	const crope& Text() const;

	int operator == (const MField& r) const;
	int operator != (const MField& r) const;
	int operator < (const MField& r) const;
	int operator > (const MField& r) const;
	operator const void* () const;
	operator ! () const;

	/**
	* The null field, it's name is "".
	*/
	static const MField	Null;
};

#endif

