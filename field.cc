//
// RFC822 Message Header Field
//

#include <mail++/field.h>
#include <mail++/parse822.h>

#include <assert.h>

//
// MField
//

int MField::Write(Rope& text, const Rope& name, const Rope& value)
{
	text.clear();

	text += name;
	text += ": ";
	text += value;
	text += "\r\n";

//		text = Fold(text_);
	return 1;
}
int MField::Read(MField::Ptr& p, const MField::Ptr& e, Rope& name, Rope& value)
{
	// Using the summary syntax from RFC821 B.1:
	//
	// field = field-name ":" [field-body] CRLF

	Ptr save = p;

	MRfc822Tokenizer lexer;

	lexer.SkipComments(p, e);

	Rope n;

	if(!lexer.GetFieldName(p, e, n)) {
		p = save;
		return 0;
	}

	Rope v;

	if(!lexer.GetFieldBody(p, e, v)) {
		p = save;
		return 0;
	}

	name	= n;
	value	= v;

	return 1;
}
MField::MField()
{
}
MField::MField(const crope& name, const crope& value) :
	name_ (name), value_(value)
{
	Write(text_, name_, value_);
}
MField::MField(Ptr& p, const Ptr& e)
{
	if(Read(p, e, name_, value_))
		Write(text_, name_, value_);
}
const crope& MField::Name() const
{
	return name_;
}
const crope& MField::Value() const
{
	return value_;
}
const crope& MField::Text() const
{
	return text_;
}
int MField::operator == (const MField& r) const
{
	return name_.compare(r.name_) == 0;
}
int MField::operator != (const MField& r) const
{
	return name_.compare(r.name_) != 0;
}
int MField::operator < (const MField& r) const
{
	return name_.compare(r.name_) < 0;
}
int MField::operator > (const MField& r) const
{
	return name_.compare(r.name_) > 0;
}
MField::operator const void* () const
{
	return (*this == Null) ? NULL : this;
}
MField::operator ! () const
{
	return *this != Null;
}
const MField MField::Null;

