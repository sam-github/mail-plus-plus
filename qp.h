//
// Quoted-Printable Encoder and Decoder Classes
//

#ifndef M_QP_H
#define M_QP_H

struct Qp
{
private:
	friend class Encoder;
	friend class Decoder;

	/**
	 * I don't want to have to deal with memory allocation/memory
	 * allocation failures, so the encoder can buffer space for the
	 * result of encoding/decoding a single character (up to 5 chars
	 * can result), but the user is responsible for reading the output
	 * back after all conversions.
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
				return Qp::EOS;
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

#if 0
	class Decoder
	{
	private:
		Qp::Buffer buffer;

		int chunk;
		int sextets;

	public:
		/**
		* Decoder is constructed in a reset state, ready to begin
		* decoding a stream of base64 encoded data. The input stream
		* must be a multiple of 4 base64 characters, [a-zA-Z0-9+/].
		* Other characters are ignored.
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
		* Resets the state of the decoder,
		*/
		void Reset();

	private:
		/**
		* Copying of Decoders doesn't make sense, and is not
		* supported.
		*/
		Decoder(const Decoder&);

		/**
		* Assignment of Decoders doesn't make sense, and is not
		* supported.
		*/
		Decoder& operator = (const Decoder&);
	};
#endif

	/**
	*/
	class Encoder
	{
	private:

		Qp::Buffer buffer;

		int		ws;
		int		maxlength; // max number of chars/line
		int		llength;

		int Ok(int c)
		{
			if((c >= '!' && c <= '<') || (c >= '>' && c <= '~'))
				return 1;
			return 0;
		}
		char Hex(int c)
		{
			c &= 0xf;

			if(c >= 10)
				return char('A' + (c - 10));
			
			return char('0' + c);
		}
		void SoftBreak()
		{
			buffer.Push('=');
			buffer.Push('\r');
			buffer.Push('\n');
			llength = 0;
		}
		void Raw(int c)
		{
			buffer.Push((char)c);

			if(maxlength) {
				llength += 1;
				if(llength >= maxlength)
					SoftBreak();
			}
		}
		void Quote(int c)
		{
			// see if this will push us over
			if(maxlength) {
				if(llength + 3 > maxlength)
					SoftBreak();
			}

			buffer.Push('=');

			char hex2 = Hex(c);
			char hex1 = Hex(c >> 4);

			buffer.Push(hex1);
			buffer.Push(hex2);

			if(maxlength) {
				llength += 3;
				if(llength == maxlength)
					SoftBreak();
			}
		}
		void Append(int c)
		{
			if(Ok(c))
				Raw(c);
			else
				Quote(c);
		}

	public:
		/**
		* Constructs an encoder with a default line length.
		* @param maxlength is the line length of the base64 encoded data,
		*                  where 0 means don't line wrap.
		*/
		Encoder(int maxlength = Qp::RFCLL);
		void Push(int c = Qp::EOS);
		int Pop();
		void Reset();

	private:
		/**
		* Copying of Encoders doesn't make sense, and is not
		* supported.
		*/
		Encoder(const Encoder&);

		/**
		* Assignment of Encoders doesn't make sense, and is not
		* supported.
		*/
		Encoder& operator = (const Encoder&);
	};
};

#endif

