//
// qp.cc
//

#include <mail++/qp.h>

#if 0
Qp::Decoder::Decoder() :
		chunk		(0),
		sextets		(0)
{
}
void Qp::Decoder::Push(int c)
{
	if(c == Qp::EOS) {
		// There's not actually anything to do in this case, the
		// input must be a complete base64 encoded stream, or it
		// isn't decoded correctly.
	} else {
		int b = base64_decode[c & 0xff];
		if(b == 255) {
			// If c is not in the base64 alphabet, ignore it. CRLF
			// pairs and white space are normal, other chars may
			// indicate a corrupted message, but there's not much
			// we can do about that.
			return;
		}

		switch(sextets) {
		case 0:
			chunk = b << 2;
			sextets = 1;
			break;
		case 1:
			chunk |= b >> 4;
			buffer.Push(char(	chunk 	));
			chunk = (b & 0xf) << 4;
			sextets = 2;
			break;
		case 2:
			chunk |= b >> 2;
			buffer.Push(char(	chunk	));
			chunk = (b & 0x3) << 6;
			sextets = 3;
			break;
		case 3:
			chunk |= b;
			buffer.Push(char(	chunk 	));
			sextets = 0;
			break;
		}
	}
}
int Qp::Decoder::Pop()
{
	return buffer.Pop();
}
void Qp::Decoder::Reset()
{
	// Initialize to just-constructed state.
	new(this) Decoder;
}
#endif

Qp::Encoder::Encoder(int maxl) :
		ws			(0),
		maxlength	(maxl),
		llength		(0)
{
	if(maxlength > Qp::RFCLL) { maxlength = Qp::RFCLL; }
	if(maxlength > 0 && maxlength < 4)
		maxlength = 4;

//	maxlength--;
}
void Qp::Encoder::Push(int c)
{
	if(c == Qp::EOS) {
		if(ws) {
			llength -= 3;
			Quote(ws);
		}
	} else {
		switch(ws)
		{
		case ' ':
		case '\t':
			switch(c)
			{
			case ' ':
			case '\t':
				Raw(ws);
				ws = c;
				break;
			case '\r':
				// this will quote all ws before a CR whether or not that
				// CR is part of a CRLF line ending, which is not required,
				// but does no harm, and won't be unsightly given the
				// scarcity of a lone CR in text
				Quote(ws);
				ws = '\r';
				break;
			default:
				Raw(ws);
				ws = 0;

				Append(c);
				break;
			}
			break;
		case '\r':
			if(c == '\n') {
				buffer.Push('\r');
				buffer.Push('\n');
				llength = 0;
				ws = 0;
				break;
			} else {
				Quote('\r');
				ws = 0;
				// drop through, treat as a normal character
			}

		default:
			switch(c)
			{
			// defer whitespace and CR until we see the next character
			case ' ':
			case '\t':
			case '\r':
				ws = c;
				break;

			default:
				Append(c);
				break; 
			}
			break;
		}
	}
}
int Qp::Encoder::Pop()
{
	return buffer.Pop();
}
void Qp::Encoder::Reset()
{
	// initialize to default state
	new(this) Encoder(maxlength);
}

