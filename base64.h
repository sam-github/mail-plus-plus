//
// Base64 Encoder and Decoder Classes
//

#ifndef M_BASE64_H
#define M_BASE64_H

struct Base64
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

		void LlInc()
		{
			if(maxlength) {
				llength++;
				if(llength == maxlength) {
					buffer.Push('\r');
					buffer.Push('\n');
					llength = 0;
				}
			}
		}

	public:
		/**
		* Constructs an encoder with a default line length.
		* @param maxlength is the line length of the base64 encoded data,
		*                  where 0 means don't line wrap.
		*/
		Encoder(int maxlength = Base64::RFCLL);
		void Push(int c = Base64::EOS);
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

