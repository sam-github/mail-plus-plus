//
// RFC822 Message Header
//

#include <mail++/header.h>

#include <assert.h>

MHeader::MHeader()
{
}
MHeader::MHeader(const crope& text)
{
	Text(text);
}

const MField&  MHeader::Field(int fieldno) const
{
	MFields::const_iterator p = fields_.begin() + fieldno;
	if(p != fields_.end()) {
		return *p;
	}
	return MField::Null;
}
const MField&  MHeader::Field(const crope& name) const
{
	MField f(name, "");

	for(MFields::const_iterator p = fields_.begin();
		p != fields_.end();
		++p)
	{
		if(f == *p)
			return *p;
	}
	
	return MField::Null;
}
const crope& MHeader::Text() const
{
	// this is a conceptually const operation

	MHeader& h = (MHeader&) *this;

	return h.Text();
}
const crope& MHeader::Text()
{
	if(IsDirty())
	{
		text_.clear();
		for(MFields::const_iterator p = fields_.begin();
			p != fields_.end();
			++p)
		{
			text_ += p->Text();
		}
	}

	return text_;
}

const crope& MHeader::Text(const crope& text)
{
	text_ = text;

	ROUT(text_);

	crope::const_iterator p = text_.begin();
	crope::const_iterator e = text_.end();

	do {
		MField	field(p, e);

		ROUT(field.Name());

		if(!field)
			break;

		fields_.push_back(field);

	} while(p != e);

	return text_;
}
const MField& MHeader::Field(const crope& name, const crope& value)
{
	MField f(name, value);

	for(MFields::iterator p = fields_.begin();
		p != fields_.end();
		++p)
	{
		if(f == *p)
			break;
	}
	if(p == fields_.end())
		fields_.push_back(f);
	else
		*p = f;

	Dirty();

	return Field(name);
}

