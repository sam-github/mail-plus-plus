//
// Mime Application Type
//

#ifndef M_APPLICATION_H
#define M_APPLICATION_H

#include <mail++/rope.h>
#include <mail++/mime.h>

class MApplication
{
private:
	typedef crope Rope;
	typedef Rope::const_iterator Ptr;

	static const Rope type_;

	Rope    subtype_;

	Rope	text_;

	Rope	encoded_text_;
	MEncoding	encoding_;

public:
	MApplication(const Rope& subtype = "octet-stream");
	MApplication(const Rope& subtype, const Rope& text);

		// accessors
	const Rope&		Type() const;
	const Rope&		SubType() const;
	const Rope&		Text() const;
	MMessage&		Fill(MMessage& mail) const;

		// modifiers
	const Rope&		Text(const Rope& text);
};

#endif

