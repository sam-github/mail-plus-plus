//
// Mime Text Type
//

#include <mail++/text.h>
#include <mail++/message.h>
#include <mail++/encoding.h>

const MText::Rope MText::type_ = "text";

MText::MText(const Rope& subtype) :
	subtype_ (subtype)
{
}
MText::MText(const Rope& subtype, const Rope& text) :
	subtype_ (subtype)
{
	Text(text);
}

	// accessors
const MText::Rope& MText::Type() const
{
	return type_;
}
const MText::Rope& MText::SubType() const
{
	return subtype_;
}
const MText::Rope& MText::Text() const
{
	return text_;
}
MMessage& MText::Fill(MMessage& mail) const
{
	MContentType content(type_, subtype_);

	if(!charset_.empty())
		content.Parameter("charset", charset_);

	mail.Head().Field(content.FieldName(), content.Text());

	mail.Head().Field(encoding_.FieldName(), encoding_.Text());

	MSetMimeVersion(mail);

	mail.Body(encoded_text_);

	return mail;
}
const MText::Rope& MText::Text(const Rope& text)
{
	text_ = text;

	encoding_ = MDataType::Encode(encoded_text_, text_);

	return text_;
}

