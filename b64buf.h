//
// b64buf.h
//
// $Id$

#ifndef M_B64BUF_H
#define M_B64BUF_H

#ifdef B64BUF_LOG
#	include <stdarg.h>
#	include	<stdio.h>
#endif

#include <iostream.h>

#include <mail++/base64.h>

class b64decbuf : public streambuf
{
private:
	Base64::Decoder dec;

	char buffer[10];

	streambuf* sb;

#ifdef B64BUF_LOG
	void Log(const char* format, ...)
	{
		va_list arglist;
		va_start(arglist, format);
		fprintf(stderr, "b64decbuf::");
		vfprintf(stderr, format, arglist);
		fprintf(stderr, "\n");
		va_end(arglist );
	}
#else
	void Log(const char* format, ...)
	{
		format = format;
	}
#endif

public:
	/**
	* @param sb, a pointer to a streambuf to use as the source of encoded
	*   data, or the sink for decoded data.
	*/
	b64decbuf(streambuf* sb) : sb(sb)
	{
		Log("b64decbuf(%#x)", sb);
	}
	~b64decbuf()
	{
		Log("~b64decbuf() sb %#x", sb);

		overflow(EOF);
	}
	int overflow(int c)
	{
		Log("overflow() c %c", c);
		dec.Push(c);
		for(int o = dec.Pop(); o != EOF; o = dec.Pop()) {
			Log("overflow() o %c");
			if(sb->sputc(o) == EOF) {
				Log("overflow() sb->sputc() -> EOF");
				return EOF;
			}
		}
		return 0;
	}
	int underflow()
	{
		// Decode the next characters until we get a character
		// out of the decoder, it could require pushing a few
		// chars into it.

		Log("underflow()");

		int i = 0;
		while(i == 0) {
			int c = sb->sgetchar();

			if(c == EOF) {
				Log("underflow() sb->sgetchar() -> EOF");
				break;
			}

			dec.Push(c);

			while((c = dec.Pop()) != EOF) {
				buffer[i] = (char) c;
				i++;
			}
		}
		Log("underflow() i %d", i);

		if(i > 0) {
			setg(buffer, buffer, buffer+i);
			return buffer[0];
		}
		setg(0, 0, 0);
		return EOF;
	}
	/**
	* @return EOF, we always use our own internal buffer.
	*/
	int doallocate()
	{
		Log("doallocate()");
		return EOF;
	}
	/**
	* The default implementation.
	*/
	int sync()
	{
		int r = sb->sync();
		Log("sync() sb->sync() -> %d", r);
		return r;
	}
	/**
	* @return NULL, we always use our own internal buffer.
	*/
	int setbuf()
	{
		Log("setbuf()");
		return NULL;
	}
};

class b64encbuf : public streambuf
{
private:
	Base64::Encoder enc;

	char buffer[10];

	streambuf* sb;

#ifdef B64BUF_LOG
	void Log(const char* format, ...)
	{
		va_list arglist;
		va_start(arglist, format);
		fprintf(stderr, "b64encbuf::");
		vfprintf(stderr, format, arglist);
		fprintf(stderr, "\n");
		va_end(arglist );
	}
#else
	void Log(const char* format, ...)
	{
		format = format;
	}
#endif

public:
	/**
	* Constructs an encoder with sb as it's source or sink. Writing
	* to the encoding streambuf causes the data to be encoded and
	* written to the provided sb, while reading from the encoding
	* streambuf causes data to be read from the provided sb and encoded.
	*
	* @param sb, the streambuf used as a source of data to encode, or a
	*     sink for encoded data.
	* @param maxlength, the length of lines that the encoded data will
	*     be broken into, if greater than zero. Encoded data is usually
	*     used in MIME mail, and must be broken into CRLF seperated
	*     lines with a max length of 76 chars, if maxlength is greater
	*     than that it will be truncated.
	*/
	b64encbuf(streambuf* sb, int maxlength = 0) : enc(maxlength), sb(sb)
	{
		Log("b64decbuf(%#x)", sb);
	}
	~b64encbuf()
	{
		Log("~b64decbuf()");
		sync();
	}
	int overflow(int c)
	{
		Log("overflow() c %c", c);

		enc.Push(c);
		for(int o = enc.Pop(); o != EOF; o = enc.Pop()) {
			if(sb->sputc(o) == EOF) {
				return EOF;
			}
		}
		return 0;
	}
	int underflow()
	{
		// Encode the next character.
		int c = sb->sgetchar();

		Log("underflow() sb->getchar() %c", c);

		enc.Push(c);

		int i = 0;
		while((c = enc.Pop()) != EOF) {
			buffer[i] = (char) c;
			i++;
		}
		Log("overflow() i %c", i);
		if(i > 0) {
			setg(buffer, buffer, buffer+i);
			return buffer[0];
		}
		setg(0, 0, 0);
		return EOF;
	}
	/**
	* @return EOF, we always use our own internal buffer.
	*/
	int doallocate()
	{
		Log("doallocate()");
		return EOF;
	}
	/**
	* Encoding requires terminating the stream, sync does this,
	* writing any of the last characters generated.
	* @return 0, success.
	*/
	int sync()
	{
		Log("sync()");

		overflow(EOF);

		int r = sb->sync();
		Log("sync() sb->sync() -> %d", r);
		return r;
	}
	/**
	* @return NULL, we always use our own internal buffer.
	*/
	int setbuf()
	{
		Log("setbuf()");
		return NULL;
	}
};

#endif

