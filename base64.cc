#include <assert.h>
#include <stdio.h>
#include <strstream.h>

#include "base64_table.c"

class Base64Encoder
{
private:
public:
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

	int		chunk;
	int		octets;
	int		maxlength; // max number of chars/line
	int		llength;

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
			switch(octets) {
			case 1:
				buffer.Push(base64_encode[chunk << 4]);
				buffer.Push('=');
				buffer.Push('=');
				break;
			case 2:
				buffer.Push(base64_encode[chunk << 2]);
				buffer.Push('=');
			case 0:
				break;
			}

			buffer.Push('\r');
			buffer.Push('\n');
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
	int Pop()
	{
		return buffer.Pop();
	}

};


int main()
{
	const int maxlength = 59; // max number of chars/line
	int	llength = 0;

	const void * vp = &base64_decode[0]; vp = vp;

	Base64Encoder enc;

	while(!feof(stdin)) {
		enc.Push(getchar());

		for(int o = enc.Pop(); o != EOF; o = enc.Pop()) {
			printf("%c", o);
		}
	}

	return 0;
}

