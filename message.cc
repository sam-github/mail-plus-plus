//
//
//

#include "msg_822.h"

#include <assert.h>

inline void rr(const crope& _)
{
	crope r = _;
	const char* s = r.c_str();
	r.delete_c_str();
}

inline void rr(const crope::const_iterator& b, const crope::const_iterator& e)
{
	crope r(b, e);
	const char* s = r.c_str();
	r.delete_c_str();
}

//
// MField
//

	int MField::StrCmp(const crope& l, const crope& r)
	{
		struct CaseCmp
		{
			int operator () (char l, char r)
			{
				char ls[2] = { l, 0 };
				char rs[2] = { r, 0 };
				return stricmp(ls, rs);
			}
		};
		rr(l);
		rr(r);
		crope::const_iterator
			lb = l.begin(),
			le = l.end(),
			rb = r.begin(),
			re = r.end();

		// The following expression is copied from the SGI docs for
		// lexicographical_compare_3way():
		return
			lexicographical_compare(lb,le,rb,re,CaseCmp()) ?
				-1 : (lexicographical_compare (rb,re,lb,le,CaseCmp()) ?
					1 : 0);
	}

	MField::MField()
	{
	}
	MField::MField(const crope& name, const crope& value) :
		name_ (name), value_(value)
	{
		text_ += name;
		text_ += ": ";
		text_ += value;
		text_ += "\r\n";

//		MField::Fold(text_);
	}

	MField::Ptr MField::Read(const MField::Ptr& begin, const MField::Ptr& end)
	{
		// nullify this field, in case it isn't already
		if(*this)
			*this = Null;

		// find the field-name
		Ptr p = find(begin, end, ':');
		if(p == end)
			return begin;

		name_.append(begin, p);

		// move past the ':'
		++p;

		// find the field-body
		const char* sep = "\r\n";
		while(1)
		{
//			rr(p, end);
#if 1
			Ptr eol = p;
			while(eol != end) {
				char c = *eol;
				if(eol[0] == '\r' && eol[1] == '\n')
					break;
				++eol;
			}
#else
			Ptr eol = search(p, end, sep, sep + strlen(sep));
#endif

			value_.append(p, eol);

			p = eol;

			if(eol == end)
				break;	// no more, so we're done

			assert(p[0] == '\r');
			assert(p[1] == '\n');
			p += 2;

			if(p == end)
				break;	// no more, so we're done

			// check if next line is a continuation line
			if(*p != ' ' && *p != '\t')
				break;
		}
		// snap the whole thing into text
		text_.append(begin, p);

		rr(p, end);

		return p;
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
		return StrCmp(Name(), r.Name()) == 0;
	}
	int MField::operator != (const MField& r) const
	{
		return StrCmp(Name(), r.Name()) != 0;
	}
	int MField::operator < (const MField& r) const
	{
		return StrCmp(Name(), r.Name()) < 0;
	}
	int MField::operator > (const MField& r) const
	{
		return StrCmp(Name(), r.Name()) > 0;
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

//
// MHeader
//

	MHeader::MHeader() {}
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
		return text_;
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

		crope::const_iterator b = text_.begin();
		crope::const_iterator e = text_.end();

		do {
			MField	field;

			b = field.Read(b, e);

//			rr(field.Name());

			if(field)
				fields_.push_back(field);

		} while(b != e);

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

//
// MMessage
//

	MMessage::MMessage()
	{
		Clean();
	}
	MMessage::MMessage(const crope& text)
	{
		Text(text);
	}
	const MHeader& MMessage::Head() const
	{
		return head_;
	}
	MHeader& MMessage::Head()
	{
		return head_;
	}
	const crope& MMessage::Body() const
	{
		return body_;
	}
	const crope& MMessage::Text() const
	{
		return text_;
	}
	const crope& MMessage::Text()
	{
		if(IsDirty()) {
			text_.clear();
			text_ += head_.Text();
			text_ += "\r\n";
			text_ += body_;
		}
		return text_;
	}
	const crope& MMessage::Body(const crope& body)
	{
		Dirty();

		body_ = body;

		return body_;
	}
	const crope& MMessage::Text(const crope& text)
	{
		text_ = text;

		Ptr	p = Search(text_.begin(), text_.end(), "\r\n\r\n");

		if(p == text_.end())
		{
			// probably an error if there's no body...
			head_.Text(text_);
			body_ = "";
		}
		else
		{
				// Head includes the crlf of the last field,
			head_.Text(crope(text_.begin(), p + 2));
				// but body doesn't include the second crlf (the blank line).
			assign(body_, p + 4, text_.end());
		}

		Clean();

		return text_;
	}

