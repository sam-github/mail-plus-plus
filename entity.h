//
// A MIME Entity
//

#ifndef M_ENTITY_H
#define M_ENTITY_H

#include <mail++/message.h>
#include <mail++/mime.h>

/**
*/
class MEntity : public MMessage
{
private:

public:
	MEntity() {}
	MEntity(const Rope& text) : MMessage(text) {}

	void Encode()
	{
	}
};

#endif

