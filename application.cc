//
// Mime Application Type
//

#include <mail++/application.h>
#include <mail++/message.h>
#include <mail++/encoding.h>

const MApplication::Rope MApplication::type_ = "application";

MApplication::MApplication(const Rope& subtype) :
	subtype_ (subtype)
{
}
MApplication::MApplication(const Rope& subtype, const Rope& text) :
	subtype_ (subtype)
{
	Text(text);
}

	// accessors
const MApplication::Rope& MApplication::Type() const
{
	return type_;
}
const MApplication::Rope& MApplication::SubType() const
{
	return subtype_;
}
const MApplication::Rope& MApplication::Text() const
{
	return text_;
}
MMessage& MApplication::Fill(MMessage& mail) const
{
	MContentType content(type_, subtype_);

	mail.Head().Field(content.FieldName(), content.Text());

	mail.Head().Field(encoding_.FieldName(), encoding_.Text());

	MSetMimeVersion(mail);

	mail.Body(encoded_text_);

	return mail;
}
const MApplication::Rope& MApplication::Text(const Rope& text)
{
	text_ = text;

	encoding_ = MDataType::Encode(encoded_text_, text_);

	return text_;
}

