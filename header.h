//
// RFC822 Message Header
//

#ifndef M_HEADER_H
#define M_HEADER_H

#include <mail++/rope.h>
#include <mail++/field.h>

#include <vector>

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

#endif

