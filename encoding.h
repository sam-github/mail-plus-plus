//
// Detect data type, and encode that data as appropriate.
//

#ifndef M_ENCODING_H
#define M_ENCODING_H

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

	EDataType	type;

public:
	enum EDataType { E_7BIT, E_8BIT, E_BINARY };

	MDataType(Ptr p, const Ptr& e) :
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

			octets++;
			ll++;

			if(c > 127) {
				eightbit++;
			} else if(c == 0) {
				null++;
			} else if(c == '\n') {
				nl++;
			} else if(c == '\r') {
				// followed by a NL it's a crlf, else it's just a cr
				Ptr n = p + 1;
				if(n != e && *n == '\n') {
					p = n;
					crlf++;
					octets++;
					ll++;

					if(ll > maxll)
						maxll = ll;

					ll = 0;
				} else {
					cr++;
				}
			}

			if((c >= 33 && c <= 60) || ((c >= 62 && c <= 126))
				qpok_++;
		}

		type = E_BINARY; // unless we prove otherwise

		if(maxll <= 998 && !null_ && !cr_ && !nl_) {
			type = E_8BIT;

			if(!eightbit_)
				type = E_7BIT;
		}
	}

	EDataType DataType() const { return type_; }

	/**
	*/
	const MEncoding& UseEncoding() const
	{
		// The aproximate size of a quoted-printable encoding, ignoring
		// breaking of long lines and quoting of '=' signs.

		int qpsize = qpok_ + (octets_ - qpok_ - crlf) * 3;
		int b64size = (octets_ / 3) * 4;

		switch(type_)
		{
		case E_BINARY:
			if(qpsize < ((b64size * 9) / 10))

		}
	}
};

#endif

