//
// Base64 Encoder and Decoder Classes
//
// $Id$
// $Log$
// Revision 1.1  1999/11/07 17:33:12  sam
// Initial revision
//

#ifndef BASE64_H
#define BASE64_H

struct Base64
{
private:
	friend class Encoder;
	friend class Decoder;

	class Buffer
	{
	public:
		Buffer() : e(0), p(0) {}
		char	buffer[10];
		int		e;
		int		p;
		void Push(char c) {
			buffer[e++] = c;
		}
		int Pop() {
			if(p == e) {
				e = p = 0;
				return Base64::EOS;
			}
			return buffer[p++];
		}
	};

public:
	/**
	* Used to indicate end-of-stream for the input or output stream.
	* It is value-equivalent to EOF in <stdio>.
	*/
	enum { EOS = -1 };

	/**
	* The maximum length of a line in an RFC compliant mail message,
	* not including the line-ending <CR><LF> pair.
	*/
	enum { RFCLL = 76 };

	/**
	* Used to encode an 8-bit byte-stream into a base64 encoded 7-bit
	* ASCII stream, suitable for transit through the internet mail
	* system.
	*/
	class Encoder
	{
	private:

		Base64::Buffer buffer;

		int		chunk;
		int		octets;
		int		maxlength; // max number of chars/line
		int		llength;

	public:
		/**
		* Constructs an encoder with a default line length.
		* @param maxlength The nMailers
		* tend to encode binary objects with a line length somewhat
		* shorter than the max allowed by the relevant RFCs, so this
		* default.
		*/
		Encoder(int maxlength = 59);
		void Push(int c = Base64::EOS);
		int Pop();

	private:
		/**
		* Copy construction of Encoders doesn't make sense, and is not
		* supported.
		*/
		Encoder(const Encoder& encoder);

		/**
		* Assignment of Encoders doesn't make sense, and is not
		* supported.
		*/
		Encoder& operator = (const Encoder& encoder);
	};
};

#endif

