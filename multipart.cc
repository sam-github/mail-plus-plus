//
// Mime Multipart Type
//

#include <mail++/multipart.h>

const MMultipart::Rope MMultipart::type_ = "multipart";

//	MMultipart();
MMultipart::MMultipart(const Rope& subtype) :
	subtype_	(subtype),
	boundary_	("aaaaaa:aaaaaa:aaaaaa:aaaaaa"),
	preamble_	("This is a multipart message in MIME format.")
{
}

	// accessors
const MMultipart::Rope& MMultipart::Type() const
{
	return type_;
}
const MMultipart::Rope& MMultipart::SubType() const
{
	return subtype_;
}
const MMultipart::Rope& MMultipart::Boundary() const
{
	return boundary_;
}
const MMultipart::Rope& MMultipart::Preamble() const
{
	return preamble_;
}
int MMultipart::Entities() const
{
	return entities_.size();
}
const MEntity& MMultipart::Entity(int index) const
{
	return *(entities_.begin());
}
MMessage& MMultipart::Fill(MMessage& mail)
{
	MContentType content("multipart", subtype_);

	content.Parameter("boundary", boundary_);

	ROUT(content.FieldName())
	ROUT(content.Text())

	mail.Head().Field(content.FieldName(), content.Text());

	ROUT(mail.Head().Field(content.FieldName()).Text())

	MSetMimeVersion(mail);

	mail.Body(Text());

	return mail;
}
const MMultipart::Rope& MMultipart::Preamble(const Rope& preamble)
{
	preamble_ = preamble;

	return preamble_;
}
int MMultipart::Add(MEntity& entity)
{
	entities_.push_back(entity);

	return entities_.size();
}
const MMultipart::Rope&		MMultipart::Text()
{
	text_.clear();

	text_ += "\r\n"; // esthetic
	text_ += preamble_;
	text_ += "\r\n"; // esthetic

	const char* t = text_.c_str();

	if(!entities_.empty())
	{

		Ptr p = entities_.begin();
		Ptr e = entities_.end();

		while(p != e)
		{
			text_ += "\r\n--";
			text_ += boundary_;
			text_ += "\r\n";
			text_ += p->Text();

			ROUT(p->Text())

			++p;
		}
		text_ += "\r\n--";
		text_ += boundary_;
		text_ += "--\r\n";
		const char* t = text_.c_str();
	}
	return text_;
}

