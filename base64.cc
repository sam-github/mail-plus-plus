#include <assert.h>
#include <stdio.h>
#include <strstream.h>

#include "base64_table.c"

	union B64Chunk
	{
	/*
		// Used to uncover the host bit-order...
		struct {
			int a : 1;
			int b : 1;
			int c : 1;
			int d : 1;
			int e : 1;
			int f : 1;
			int g : 1;
			int h : 1;
		} b2[3];
		struct {
			char	octet[3];
			char& operator [] (int index)
			{
				assert(index >= 0 && index < 3);
				int i;
				switch(index)
				{
				case 2: i = 0; break;
				case 1: i = 1; break;
				case 0: i = 2; break;
				}
				return octet[i];
			}
		}  b256;
	*/
		char b256[3];
		struct {
			int	d : 6;
			int	c : 6;
			int	b : 6;
			int	a : 6;
/*
			char operator [] (int index)
			{
				assert(index >= 0 && index < 4);
				switch(index)
				{
				case 0: return a;
				case 1: return b;
				case 2: return c;
				case 3: return d;
				}
				return -1;
			}
*/
		} b64;
	};


/*
class Base64Encoder
{
private:
public:
	B64Chunk chunk;
	int		octet;

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
				return EOF;
			}
			return buffer[p++];
		}
	};
	Buffer buffer;

	const int maxlength; // max number of chars/line
	int	llength;
	int octets;

	void Convert()
	{
		if(octets == 0)
			return;

		// write enough b64s to include all the octets
		int b64s = ((octets * 8) / 6) + (((octets * 8) % 6) ? 1 : 0);

		// print the four b64 encoded chars, perhaps with padding
		for(int i = 0; i < 4; i++) {
			if(i < b64s) {
				char b = chunk.b64[i];
				char e = base64_encode[b];
				buffer.Push(e);
			} else {
				buffer.Push('=');
			}
			llength++;
			if(llength > maxlength) {
				buffer.Push('\r');
				buffer.Push('\n');
				llength = 0;
			}
		}
	}

public:
	Base64Encoder() :
			octets		(0),
			maxlength	(59),
			llength		(0)
	{
	}

	void Push(int c)
	{
		if(c == EOF) {
			Convert();
		} else {
			chunk.b256[octets] = (char)c;
			if(++octets == 3) {
				Convert();
			}
		}
	}
	int Pop()
	{
		return buffer.Pop();
	}

};
*/

int main()
{
	const int maxlength = 59; // max number of chars/line
	int	llength = 0;

	const void * vp = &base64_decode[0]; vp = vp;

		struct {
			int	d : 6;
			int	c : 6;
			int	b : 6;
			int	a : 6;
		} _64;
	printf("_64 %d\n", sizeof(_64));

	B64Chunk __;
//	size_t b2	= sizeof(__.b2);
	size_t b256	= sizeof(__.b256);
	size_t b64	= sizeof(__.b64);
	printf("b64 %d b256 %d\n", b64, b256);
	assert(b256 == b64);

	return 0;
#if 0
	Base64Encoder enc;

	while(1) {
		int i = getchar();
		enc.Push(i);

		int o;
		while((o = enc.Pop()) != EOF) {
			printf("%c", o);
		}
		if(i == EOF)
			break;
	}
	printf("\r\n");

	return 0;
#endif

#if 0
	do {
		B64Chunk chunk;
		int octets;
		int	b64s;
		int	i;

		chunk.b256[0] = 0;
		chunk.b256[1] = 0;
		chunk.b256[2] = 0;

		for(octets=0; octets<3; octets++) {
			int c = getchar();
			if(c == EOF) {
				// pad the last b64 block with zero bits
				chunk.b256[octets] = 0;
				break;
			}
			chunk.b256[octets] = c;
		}
		if(octets == 0)
			continue;

		/* write enough b64s to include all the octets */
		b64s = ((octets * 8) / 6) + (((octets * 8) % 6) ? 1 : 0);

		/* print the four b64 encoded chars, perhaps with padding */
		for(i = 0; i < 4; i++) {
			if(i < b64s) {
				char b = chunk.b64[i];
				char e = base64_encode[b];
				printf("%c", e);
			} else {
				printf("=");
			}
			llength++;
			if(llength > maxlength) {
				printf("\r\n");
				llength = 0;
			}
		}

	} while(!feof(stdin) && !ferror(stdin));

	printf("\r\n");

	return 0;
#endif
}

