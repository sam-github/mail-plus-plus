#include <stdio.h>
#include <ctype.h>

int main()
{
	for(int c = 0; c <= 255; ++c)
	{
		if(isprint(c)) {
			printf("%#2x: '%c'\n", c, c);
		}
	}
	return 0;
};

