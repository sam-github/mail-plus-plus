//
// Mime Header Fields
//

#include <mail++/mime.h>
#include <mail++/message.h>
#include <mail++/parse2045.h>
#include <mail++/quote.h>

#include <new.h>
#include <strstream.h>

//
// Utility Functions
//

int MSetMimeVersion(MMessage& message, int major, int minor)
{
	MMimeVersion v(major, minor);

	return message.Head().Field("MIME-Version", v.Text())
				!= MField::Null;
}
int MSetContentType(MMessage& message,
		const crope& type, const crope& subtype)
{
	MContentType content(type, subtype);

	return message.Head().Field("Content-Type", content.Text())
				!= MField::Null;
}

//
// MimeVersion
//

int MMimeVersion::Compare(const MMimeVersion& r) const
{
	// Try and compare based on major first, then based on the minor.

	if(major_ > r.major_)
		return 1;
	if(major_ < r.major_)
		return -1;

	if(minor_ > r.minor_)
		return 1;
	if(minor_ < r.minor_)
		return -1;

	return 0;
}
void MMimeVersion::Rewrite()
{
	strstream ss;

	ss << major_ << "." << minor_ << '\0';

	text_ = ss.str();

	delete[] ss.str();
}
int MMimeVersion::Read(Ptr& p, const Ptr& e, int& major, int& minor)
{
	// version = "mime-version" ":" 1*DIGIT "." 1*DIGIT
	//
	// I'm only parsing value side of the field.

	Ptr save = p;

	MRfc822Tokenizer lexer;

	lexer.SkipComments(p, e);

	int maj;

	if(!lexer.GetDigits(p, e, 1, 0, maj)) {
		p = save;
		return 0;
	}

	lexer.SkipComments(p, e);

	if(!lexer.GetSpecial(p, e, '.')) {
		p = save;
		return 0;
	}

	lexer.SkipComments(p, e);

	int min;

	if(!lexer.GetDigits(p, e, 1, 0, min)) {
		p = save;
		return 0;
	}

	major = maj;
	minor = min;

	return 1;
}
MMimeVersion::MMimeVersion() :
	major_	(0),
	minor_	(0)
{
}
MMimeVersion::MMimeVersion(int major, int minor) :
	major_	(major),
	minor_	(minor)
{
	if(major_ > 0 && minor_ >= 0)
		Rewrite();
	else
		*this = Null;
}
MMimeVersion::MMimeVersion(Ptr& p, const Ptr& e) :
	major_	(0),
	minor_	(0)
{
	if(!Read(p, e, major_, minor_))
		return;

	if(major_ > 0 && minor_ > 0)
		Rewrite();
	else
		*this = Null;
}
MMimeVersion::MMimeVersion(const Rope& text) :
	major_	(0),
	minor_	(0)
{
	Ptr p = text.begin();
	Ptr e = text.end();

	new(this) MMimeVersion(p, e);
}
int MMimeVersion::Major() const
{
	return major_;
}
int MMimeVersion::Minor() const
{
	return minor_;
}
const MMimeVersion::Rope& MMimeVersion::Text() const
{
	return text_;
}
int MMimeVersion::operator == (const MMimeVersion& r) const
{
	return Compare(r) == 0;
}
int MMimeVersion::operator != (const MMimeVersion& r) const
{
	return Compare(r) != 0;
}
int MMimeVersion::operator < (const MMimeVersion& r) const
{
	return Compare(r) < 0;
}
int MMimeVersion::operator > (const MMimeVersion& r) const
{
	return Compare(r) > 0;
}
MMimeVersion::operator const void* () const
{
	return Compare(Null) ? this : 0;
}
MMimeVersion::operator ! () const
{
	return (Compare(Null) == 0) ? 1 : 0;
}

const MMimeVersion MMimeVersion::Null;

//
// Parameter
//

int MParameter::Write(Rope& text, const Rope& name, const Rope& value)
{
	// check name is valid
	// check value is valid, and quote it if necessary

	text += "; ";
	text += name;
	text += "=";

	crope v = value;
	MParameterQuoter q;
	q.Quote(v);

	text += v;

	return 1;
}
int MParameter::Read(Ptr& begin, const Ptr& end, Rope& name, Rope& value)
{
	MRfc2045Parser parser;

	return parser.GetParameter(begin, end, name, value);
}
MParameter::MParameter()
{
}
MParameter::MParameter(const crope& name, const crope& value) :
		name_	(name),
		value_	(value)
{
	if(!Write(text_, name_, value_))
		*this = Null;
}
MParameter::MParameter(Ptr& begin, const Ptr& end)
{
	if(Read(begin, end, name_, value_)) {
		if(!Write(text_, name_, value_))
			*this = Null;
	}
}
const crope& MParameter::Name() const
{
	return name_;
}
const crope& MParameter::Value() const
{
	return value_;
}
const crope& MParameter::Text() const
{
	return text_;
}
int MParameter::operator == (const MParameter& r) const
{
	return RopeCaseCmp(name_, r.name_) == 0;
}
int MParameter::operator != (const MParameter& r) const
{
	return RopeCaseCmp(name_, r.name_) != 0;
}
int MParameter::operator < (const MParameter& r) const
{
	return RopeCaseCmp(name_, r.name_) < 0;
}
int MParameter::operator > (const MParameter& r) const
{
	return RopeCaseCmp(name_, r.name_) > 0;
}
MParameter::operator const void* () const
{
	return (*this != Null) ? this : 0;
}
MParameter::operator ! () const
{
	return (*this == Null) ? 1 : 0;
}

static const MParameter MParameter::Null;

//
// ContentType
//

int MContentType::EqualTo(const MContentType& r) const
{
	if(RopeCaseCmp(type_, r.type_) != 0)
		return 0;

	if(RopeCaseCmp(subtype_, r.subtype_) != 0)
		return 0;

	return 1;
}
int MContentType::True() const
{
	return !EqualTo(Null);
}
int MContentType::Index(const Rope& name) const
{
	int sz = params_.size();
	int i = 0;
	for(; i < sz; ++i)
	{
		if(RopeCaseCmp(params_[i].Name(), name) == 0)
			break;
	}
	return i;
}
void MContentType::Rewrite()
{
	RopeLower(type_);
	RopeLower(subtype_);

	if(!Write(text_, type_, subtype_, params_)) {
		*this = Null;
	}
}
int MContentType::Write(Rope& text, const Rope& t, const Rope& st, const Params& pl)
{
	// check that type and subtype are both valid

	text += t;
	text += '/';
	text += st;

	int sz = pl.size();
	for(int i = 0; i < sz; ++i)
		text += pl[i].Text();

	return 1;
}
int MContentType::Read(Ptr& p, const Ptr& e, Rope& type, Rope& subtype, Params& params)
{
	MRfc2045Parser parser;

	if(!parser.GetContent(p, e, type, subtype))
		return 0;

	Rope	attr;
	Rope	value;

	while(parser.GetParameter(p, e, attr, value)) {
		params.push_back(MParameter(attr, value));
	}
	
	return 1;
}
MContentType::MContentType()
{
}
MContentType::MContentType(const Rope& type, const Rope& subtype) :
	type_	(type),
	subtype_(subtype)
{
	Rewrite();
}
MContentType::MContentType(const Rope& type, const Rope& subtype, const Params& params) :
	type_	(type),
	subtype_(subtype),
	params_	(params)
{
	Rewrite();
}
MContentType::MContentType(Ptr& p, const Ptr& e)
{
	if(Read(p, e, type_, subtype_, params_)) {
		Rewrite();
	}
}
MContentType::MContentType(const Rope& text)
{
	Ptr p = text.begin();
	Ptr e = text.end();
	if(Read(p, e, type_, subtype_, params_)) {
		Rewrite();
	}
}
const MContentType::Rope& MContentType::Type() const
{
	return type_;
}
const MContentType::Rope& MContentType::SubType() const
{
	return subtype_;
}
const MParameter& MContentType::Parameter(int index) const
{
	int sz = params_.size();
	if(index < 0 || index >= params_.size())
		return MParameter::Null;

	return params_[index];
}
const MParameter& MContentType::Parameter(const Rope& name) const
{
	int index = Index(name);

	if(index == params_.size())
		return MParameter::Null;

	return params_[index];
}
const MParameter&	MContentType::Parameter(const Rope& name, const Rope& value)
{
	int index = Index(name);

	MParameter p(name, value);

	if(index == params_.size())
		params_.push_back(p);
	else
		params_[index] = p;

	Rewrite();

	return Parameter(name);
}
const MContentType::Rope&			MContentType::Text() const
{
	return text_;
}

int MContentType::operator == (const MContentType& r) const
{
	return EqualTo(r);
}
int MContentType::operator != (const MContentType& r) const
{
	return !EqualTo(r);
}
MContentType::operator const void* () const
{
	return True() ? this : 0;
}
MContentType::operator ! () const
{
	return True() ? 0 : 1;
}

const MContentType MContentType::Null;


//
// MEncoding
//

MEncoding::MEncoding()
{
}
MEncoding::MEncoding(const Rope& encoding) 
{
	MRfc2045Parser parser;

	Ptr p = encoding.begin();
	Ptr e = encoding.end();

	if(parser.GetEncoding(p, e, encoding_))
	{
		text_ = encoding_;
		RopeLower(text_);
	}
}
MEncoding::MEncoding(Ptr& p, const Ptr& e)
{
	MRfc2045Parser parser;

	if(parser.GetEncoding(p, e, encoding_))
	{
		text_ = encoding_;
		RopeLower(text_);
	}
}
const MEncoding::Rope&	MEncoding::Encoding() const
{
	return encoding_;
}
const MEncoding::Rope&	MEncoding::Text() const
{
	return text_;
}
int MEncoding::operator == (const MEncoding& e)
{
	return RopeCaseCmp(encoding_, e.Encoding()) == 0;
}
int MEncoding::operator != (const MEncoding& e)
{
	return RopeCaseCmp(encoding_, e.Encoding()) != 0;
}
const MEncoding MEncoding::E7Bit = "7bit";
const MEncoding MEncoding::E8Bit = "8bit";
const MEncoding MEncoding::EBinary = "binary";
const MEncoding MEncoding::EQuotedPrintable = "quoted-printable";
const MEncoding MEncoding::EBase64 = "base64";

