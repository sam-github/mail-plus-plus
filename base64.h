//
// Base64 Encoder and Decoder Classes
//
// $Id$
// $Log$
// Revision 1.2  1999/12/13 03:07:36  sam
// implemented the decoder
//
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

	/**
	 * I don't want to have to deal with memory allocation/memory
	 * allocation failures, so the encoder can buffer space for the
	 * result of encoding a single character (up to 5 chars can result),
	 * but the user is responsible for reading the output back after all
	 * conversions.
	 */
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

	class Decoder
	{
	private:
		Base64::Buffer buffer;

		int chunk;
		int sextets;

	public:
		/**
		* Decoder is constructed in a reset state, ready to begin
		* decoding a stream of base64 encoded data.
		*/
		Decoder();
		/**
		* Push a character into the decoder, the special value EOS
		* indicates that the stream of characters to be decoded is
		* complete. Internal buffer space is minimal, so it is required
		* that the results of a character's decoding be immediately
		* popped before another character is pushed.
		* @param c	The character to be pushed, or EOS to indicate the
		*			end-of-stream.
		*/
		void Push(int c = Base64::EOS);
		/**
		* Pops the character(s) resulting from the decoding of the last
		* character pushed into the decoder.
		*/
		int Pop();
		/**
		* Queries whether end-of-stream for the conversion has
		* been reached. End-of-stream can be detected by the encoder
		* only when the '=' character is seen in the input stream,
		* otherwise EOS must be indicated by the user, see Push().
		* Further, the input stream must be a multiple of 4 base64
		* characters, [a-zA-Z0-9+/=], this is not.
		*/
		int Eos();
		/**
		* Resets the state of the decoder,
		*/
		void Reset();
	};

	/**
	* Used to encode an 8-bit byte-stream into a base64 encoded 7-bit
	* ASCII stream, suitable for transission through the internet mail
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
		* @param maxlength MUAs
		* tend to encode binary objects with a line length somewhat
		* shorter than the max allowed by the relevant RFCs, so this
		* default is somewhat shorter as well.
		*/
		Encoder(int maxlength = 59);
		void Push(int c = Base64::EOS);
		int Pop();

	private:
		/**
		* Copying of Encoders doesn't make sense, and is not
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

