//
// Mime Multipart Type
//

#ifndef M_MULTIPART_H
#define M_MULTIPART_H

#include <mail++/entity.h>

class MMultipart
{
private:
	typedef crope Rope;
	typedef vector<MEntity> List;
	typedef List::const_iterator Ptr;

	static const Rope type_;

	Rope	subtype_;
	Rope	preamble_;
	Rope	boundary_;
	Rope	text_;

	List	entities_;

public:
//	MMultipart();
	MMultipart(const Rope& subtype);

		// accessors
	const Rope&		Type() const;
	const Rope&		SubType() const;
	const Rope&		Boundary() const;
	const Rope&		Preamble() const;
	int				Entities() const;
	const MEntity&	Entity(int index) const;
	MMessage&		Fill(MMessage& mail);

		// modifiers
	const Rope&		Preamble(const Rope& pramble);
	int				Add(MEntity& entity);
	const Rope&		Text();
};

#endif

