//
// base64.cc
//
// $Id$
// $Log$
// Revision 1.4  1999/12/13 03:07:36  sam
// implemented the decoder
//
// Revision 1.3  1999/11/07 17:33:12  sam
// Repackaged and partially documented.
//

#include <mail++/base64.h>

static const char base64_encode[] = {
	/* Value	Encoding */
	/*  0 */	'A',
	/*  1 */	'B',
	/*  2 */	'C',
	/*  3 */	'D',
	/*  4 */	'E',
	/*  5 */	'F',
	/*  6 */	'G',
	/*  7 */	'H',
	/*  8 */	'I',
	/*  9 */	'J',
	/* 10 */	'K',
	/* 11 */	'L',
	/* 12 */	'M',
	/* 13 */	'N',
	/* 14 */	'O',
	/* 15 */	'P',
	/* 16 */	'Q',
	/* 17 */	'R',
	/* 18 */	'S',
	/* 19 */	'T',
	/* 20 */	'U',
	/* 21 */	'V',
	/* 22 */	'W',
	/* 23 */	'X',
	/* 24 */	'Y',
	/* 25 */	'Z',
	/* 26 */	'a',
	/* 27 */	'b',
	/* 28 */	'c',
	/* 29 */	'd',
	/* 30 */	'e',
	/* 31 */	'f',
	/* 32 */	'g',
	/* 33 */	'h',
	/* 34 */	'i',
	/* 35 */	'j',
	/* 36 */	'k',
	/* 37 */	'l',
	/* 38 */	'm',
	/* 39 */	'n',
	/* 40 */	'o',
	/* 41 */	'p',
	/* 42 */	'q',
	/* 43 */	'r',
	/* 44 */	's',
	/* 45 */	't',
	/* 46 */	'u',
	/* 47 */	'v',
	/* 48 */	'w',
	/* 49 */	'x',
	/* 50 */	'y',
	/* 51 */	'z',
	/* 52 */	'0',
	/* 53 */	'1',
	/* 54 */	'2',
	/* 55 */	'3',
	/* 56 */	'4',
	/* 57 */	'5',
	/* 58 */	'6',
	/* 59 */	'7',
	/* 60 */	'8',
	/* 61 */	'9',
	/* 62 */	'+',
	/* 63 */	'/',
	0
};



static const int base64_decode[] = {
	/* Encoding	Value */
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/* + */	62,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/* / */	63,
	/* 0 */	52,
	/* 1 */	53,
	/* 2 */	54,
	/* 3 */	55,
	/* 4 */	56,
	/* 5 */	57,
	/* 6 */	58,
	/* 7 */	59,
	/* 8 */	60,
	/* 9 */	61,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/* A */	0,
	/* B */	1,
	/* C */	2,
	/* D */	3,
	/* E */	4,
	/* F */	5,
	/* G */	6,
	/* H */	7,
	/* I */	8,
	/* J */	9,
	/* K */	10,
	/* L */	11,
	/* M */	12,
	/* N */	13,
	/* O */	14,
	/* P */	15,
	/* Q */	16,
	/* R */	17,
	/* S */	18,
	/* T */	19,
	/* U */	20,
	/* V */	21,
	/* W */	22,
	/* X */	23,
	/* Y */	24,
	/* Z */	25,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/* a */	26,
	/* b */	27,
	/* c */	28,
	/* d */	29,
	/* e */	30,
	/* f */	31,
	/* g */	32,
	/* h */	33,
	/* i */	34,
	/* j */	35,
	/* k */	36,
	/* l */	37,
	/* m */	38,
	/* n */	39,
	/* o */	40,
	/* p */	41,
	/* q */	42,
	/* r */	43,
	/* s */	44,
	/* t */	45,
	/* u */	46,
	/* v */	47,
	/* w */	48,
	/* x */	49,
	/* y */	50,
	/* z */	51,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	/*   */	255,
	0
};

Base64::Decoder::Decoder() :
		chunk		(0),
		sextets		(0)
{
}
void Base64::Decoder::Push(int c)
{
	if(c == Base64::EOS) {
		// There's not actually anything to do in this case, the
		// input must be a complete base64 encoded stream, or it
		// isn't.
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
int Base64::Decoder::Pop()
{
	return buffer.Pop();
}
Base64::Encoder::Encoder(int maxlength) :
		chunk		(0),
		octets		(0),
		maxlength	(maxlength),
		llength		(0)
{
	if(maxlength > Base64::RFCLL) { maxlength = Base64::RFCLL; }
}
void Base64::Encoder::Push(int c)
{
	if(c == Base64::EOS) {
		switch(octets) {
		case 1:
			buffer.Push(base64_encode[chunk << 4]);
			buffer.Push('=');
			buffer.Push('=');
			octets = 0;
			break;
		case 2:
			buffer.Push(base64_encode[chunk << 2]);
			buffer.Push('=');
			octets = 0;
		case 0:
			break;
		}

		if(chunk != Base64::EOS) {
			buffer.Push('\r');
			buffer.Push('\n');

			chunk = Base64::EOS;
		}
	} else {
		switch(octets) {
		case 0:
			buffer.Push(base64_encode[c >> 2]);
			chunk = c & 0x3;
			octets = 1;
			llength++;
			break;
		case 1:
			buffer.Push(base64_encode[(chunk << 4) | (c >> 4)]);
			chunk = c & 0xf;
			octets = 2;
			llength++;
			break;
		case 2:
			buffer.Push(base64_encode[(chunk << 2) | (c >> 6)]);
			buffer.Push(base64_encode[c & 0x3f]);
			octets = 0;
			llength += 2;
			break;
		}
		if(llength > maxlength) {
			buffer.Push('\r');
			buffer.Push('\n');
			llength = 0;
		}

	}
}
int Base64::Encoder::Pop()
{
	return buffer.Pop();
}

