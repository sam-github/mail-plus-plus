//
// An RFC822 format message
//

#ifndef M_MSG_822_H
#define M_MSG_822_H

#include <rope>
#include <vector>

inline istream& operator >> (istream& is, crope& r)
{
	int c;

	while((c = is.get()) != EOF) {
		r.append(c);
		if(c == '\n')
			break;
	}

	return is;
}

inline void assign(crope& to, crope::const_iterator b, crope::const_iterator e)
{
	to.clear();
	to.append(b, e);
}

class MField
{
private:
	typedef crope::const_iterator Ptr;

	crope	name_;
	crope	value_;
	crope	text_;

	static int StrCmp(const crope& l, const crope& r);

public:
	MField();
	MField(const crope& name, const crope& value);

	Ptr Read(const Ptr& begin, const Ptr& end);

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

class MHeader
{
private:
	typedef vector<MField>	MFields;

	MFields	fields_;

	crope	text_;

	int dirty_;

	void Dirty() { dirty_ = 1; }
	void Clean() { dirty_ = 0; }

public:
	MHeader();
	MHeader(const crope& text);

	// accessors
	const MField&	Field(int fieldno) const;
	const MField&	Field(const crope& name) const;
	const crope&	Text() const;
	const crope&	Text();

	int IsDirty() const { return dirty_; }

	// modifiers
	const crope&	Text(const crope& text);
	const MField&	Field(const crope& name, const crope& value);
};

class MMessage
{
private:
	MHeader	head_;
	crope	body_;
	crope	text_;

	typedef crope::const_iterator Ptr;

	Ptr Search(Ptr b, Ptr e, const char* s)
	{
		return search(b, e, s, s + strlen(s));
	}

	int dirty_;

	void Dirty() { dirty_ = 1; }
	void Clean() { dirty_ = 0; }

public:
	MMessage();
	MMessage(const crope& text);

	// accessors
	const	MHeader&	Head() const;
			MHeader&	Head();
	const	crope&		Body() const;
	const	crope&		Text() const;
	const	crope&		Text();

	int IsDirty() const { return dirty_ || head_.IsDirty(); }

	// modifiers
	const	crope&		Body(const crope& body);
	const	crope&		Text(const crope& text);

};

/**
* Convert text (line-oriented data) from LF to cannonical CRLF line endings.
* The result is not necessarily 7bit, it may need encoding.
*/
inline void MConvertLFToCRLF(crope& text)
{
	// If last line is not LF terminated, do so.
	if(!text.empty() && text.back() != '\n')
		text.append('\n');

	for(crope::iterator p = text.mutable_begin();
		p != text.mutable_end();
		++p)
	{
		if(*p == '\n')
		{
			text.insert(p, '\r');
			++p;
		}
	}
}

/**
* If last character in container is a LF, chop it off.
*/
inline void MChopLF(crope& line)
{
	if(line.empty())
		return;

	if(line.back() != '\n')
		return;

	line.pop_back();
}

#endif

