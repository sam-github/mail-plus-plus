//
// Detect data type, and encode that data as appropriate.
//

#ifndef M_ENCODING_H
#define M_ENCODING_H

#include <mail++/rope.h>
#include <mail++/base64.h>

inline crope Base64Encode(const crope& in, int ll = Base64::RFCLL)
{
	crope out;

	Base64::Encoder encoder(ll);

	for(crope::const_iterator p = in.begin(); p != in.end(); ++p)
	{
		encoder.Push(*p);
		for(int o = encoder.Pop(); o != EOF; o = encoder.Pop()) {
			out.push_back(char(o));
		}
	}
	return out;
}
inline crope QuotedPrintableEncode(const crope& in, int ll = Base64::RFCLL)
{
	return in;
}

/**
7bit - max line length is 998 octets between CRLF pairs
     - no nulls
     - no octets > 127
     - no bare CF or NL
8bit - 7bit except octets > 127 is allowed
binary - everything else

Note that Unix text will be binary, since it has bare NLs in it.
*/
class MDataType
{
public:
	enum EDataType { E_7BIT, E_8BIT, E_BINARY };

private:
	typedef crope Rope;
	typedef Rope::const_iterator Ptr;

	int		octets_;
	int		eightbit_;
	int		null_;
	int		nl_;
	int		cr_;
	int		crlf_;
	int 	maxll_;
	int		qpok_;

	EDataType	type_;

public:

	MDataType(Ptr p, const Ptr& e, int maxll = 998) :
		eightbit_	(0),
		null_		(0),
		nl_			(0),
		cr_			(0),
		crlf_		(0),
		maxll_		(0),
		qpok_		(0)
	{
		int ll = 0;

		for( ; p != e; ++p)
		{
			int c = *p;

			octets_++;
			ll++;

			if(c > 127) {
				eightbit_++;
			} else if(c == 0) {
				null_++;
			} else if(c == '\n') {
				nl_++;
			} else if(c == '\r') {
				// followed by a NL it's a crlf, else it's just a cr
				Ptr n = p + 1;
				if(n != e && *n == '\n') {
					p = n;
					crlf_++;
					octets_++;
					ll++;

					if(ll > maxll)
						maxll = ll;

					ll = 0;
				} else {
					cr_++;
				}
			}

			if((c >= 33 && c <= 60) || (c >= 62 && c <= 126))
				qpok_++;
		}

		type_ = E_BINARY; // unless we prove otherwise

		if(maxll <= 998 && !null_ && !cr_ && !nl_) {
			type_ = E_8BIT;

			if(!eightbit_)
				type_ = E_7BIT;
		}
	}

	EDataType DataType() const { return type_; }

	/**
	*/
	const MEncoding& UseEncoding() const
	{
		// The aproximate size of a quoted-printable encoding, ignoring
		// breaking of long lines and quoting of '=' signs.

		int qpsize = qpok_ + (octets_ - qpok_ - crlf_) * 3;
		int b64size = (octets_ / 3) * 4;

		switch(type_)
		{
		case E_BINARY:
			return MEncoding::EBase64;
		case E_8BIT:
			return MEncoding::EQuotedPrintable;
/*
		case E_BINARY:
		case E_8BIT:
			if(qpsize < ((b64size * 9) / 10))
				return MEncoding::EQuotedPrintable;
			else
				return MEncoding::EBase64;
			break;
*/
		case E_7BIT:
			return MEncoding::E7Bit;
		}
		// never reached, only for compiler complaints
		return MEncoding::E7Bit;
	}
	static const MEncoding& Encode(crope& out, const crope& in, int ll = Base64::RFCLL)
	{
		MDataType dt(in.begin(), in.end());

		const MEncoding& encoding = dt.UseEncoding();

		if(encoding == MEncoding::E7Bit) {
			out = in;
		} else if(encoding == MEncoding::EQuotedPrintable) {
			out = QuotedPrintableEncode(in, ll);
		} else if(encoding == MEncoding::EBase64) {
			out = Base64Encode(in, ll);
		}
		return encoding;
	}
};

#endif

