//
// RFC822 Mail Message
//

#include <mail++/message.h>

#include <assert.h>

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
	MMessage& m = (MMessage&) *this;

	return m.Text();
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

