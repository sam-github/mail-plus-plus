//
// Test Driver for Circular Buffer Class
//
// $Id$
// $Log$
// Revision 1.1  1999/12/13 02:31:50  sam
// Initial revision
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <mail++/circbuf.h>

int vOpt = 0;

void _Assert(int r, const char* s, int l)
{
	if(!r) {
		printf("%3.3d - %s\n", l, s);
		exit(1);
	}
	if(vOpt) {
		printf("%3.3d + %s\n", l, s);
	}
}
#define Assert(R)	_Assert((R), #R, __LINE__)

int main(int argc, char* argv[])
{
	for(int opt; (opt = getopt(argc, argv, "v")) != -1; )
	{
		switch(opt)
		{
		case 'v':
			vOpt++;
			break;

		default:
			fprintf(stderr, "usage: %s -v\n", argv[0]);
			exit(1);
			break;
		}
	}

	{ circ_buffer cb(-1); Assert(cb.initial() > 0); }
	{ circ_buffer cb( 0); Assert(cb.initial() > 0); }
	{ circ_buffer cb( 1); Assert(cb.initial() > 0); }
	{ circ_buffer cb( 2); Assert(cb.initial() > 0); }
	{ circ_buffer cb(30); Assert(cb.initial() > 0); }
	{ circ_buffer cb(31); Assert(cb.initial() > 0); }
	{ circ_buffer cb(32); Assert(cb.initial() > 0); }
	{ circ_buffer cb(33); Assert(cb.initial() > 0); }

	circ_buffer cb;
	char c;

	Assert(	cb.reserve() == 0	);
	Assert(	!cb.pop(c)			);
	Assert(	cb.pop() == EOF		);

	Assert(	cb.push('c') 		);
	Assert(	cb.pop() == 'c'		);

	Assert(	cb.push('c') 		);
	Assert(	cb.pop() == 'c'		);

	Assert(	cb.push('a') 		);
	Assert(	cb.push('b') 		);
	Assert(	cb.push('c') 		);
	Assert( cb.size() == 3      );
	Assert(	cb.pop() == 'a'		);
	Assert(	cb.pop() == 'b'		);
	Assert(	cb.pop() == 'c'		);
	Assert( cb.reserve() == 4	);
	Assert( cb.size() == 0      );

	Assert(	cb.push('a') 		);
	Assert(	cb.push('b') 		);
	Assert(	cb.push('c') 		);
	Assert(	cb.push('d') 		);
	Assert( cb.size() == 4      );
	Assert(	cb.pop() == 'a'		);
	Assert(	cb.pop() == 'b'		);
	Assert(	cb.pop() == 'c'		);
	Assert( cb.size() == 1      );
	Assert(	cb.push('e') 		);
	Assert(	cb.push('f') 		);
	Assert( cb.size() == 3      );
	Assert(	cb.pop() == 'd'		);
	Assert(	cb.pop() == 'e'		);
	Assert(	cb.pop() == 'f'		);
	Assert( cb.size() == 0      );
	Assert( cb.reserve() == 8	);

	Assert(	cb.push('a') 		);
	Assert(	cb.push('b') 		);
	Assert(	cb.push('c') 		);
	Assert( cb.size() == 3      );
	Assert(	cb.pop() == 'a'		);
	Assert(	cb.pop() == 'b'		);
	Assert(	cb.pop() == 'c'		);
	Assert( cb.reserve() == 8	);
	Assert( cb.size() == 0      );

	Assert(	cb.push('a') 		);
	Assert(	cb.push('b') 		);
	Assert(	cb.push('c') 		);
	Assert( cb.size() == 3      );
	Assert(	cb.pop() == 'a'		);
	Assert(	cb.pop() == 'b'		);
	Assert(	cb.pop() == 'c'		);
	Assert( cb.reserve() == 8	);
	Assert( cb.size() == 0      );

	Assert(	cb.push('a') 		);
	Assert(	cb.push('b') 		);
	Assert(	cb.push('c') 		);
	Assert( cb.size() == 3      );
	Assert(	cb.pop() == 'a'		);
	Assert(	cb.pop() == 'b'		);
	Assert(	cb.pop() == 'c'		);
	Assert( cb.reserve() == 8	);
	Assert( cb.size() == 0      );

	Assert(	cb.push('a') 		);
	Assert(	cb.push('b') 		);
	Assert(	cb.push('c') 		);
	Assert( cb.size() == 3      );
	Assert(	cb.pop() == 'a'		);
	Assert(	cb.pop() == 'b'		);
	Assert(	cb.pop() == 'c'		);
	Assert( cb.reserve() == 8	);
	Assert( cb.size() == 0      );

	Assert(	cb.push('a') 		);
	Assert(	cb.push('b') 		);
	Assert(	cb.push('c') 		);
	Assert( cb.size() == 3      );

	circ_buffer cb2 = cb;
	Assert(	cb2.push('d') 		);
	Assert(	cb2.pop() == 'a'	);
	Assert(	cb2.pop() == 'b'	);
	Assert(	cb2.pop() == 'c'	);
	Assert(	cb2.pop() == 'd'	);
	Assert( cb2.reserve() == 8	);
	Assert( cb2.size() == 0     );

	circ_buffer cb3; cb3 = cb;
	Assert(	cb3.push('d') 		);
	Assert(	cb3.pop() == 'a'	);
	Assert(	cb3.pop() == 'b'	);
	Assert(	cb3.pop() == 'c'	);
	Assert(	cb3.pop() == 'd'	);
	Assert( cb3.reserve() == 8	);
	Assert( cb3.size() == 0     );

	circ_buffer cb4;
	Assert(	cb4.push('a') 		);
	Assert(	cb4.push('b') 		);
	Assert(	cb4.push('c') 		);
	cb4 = cb;
	Assert(	cb4.push('d') 		);
	Assert(	cb4.pop() == 'a'	);
	Assert(	cb4.pop() == 'b'	);
	Assert(	cb4.pop() == 'c'	);
	Assert(	cb4.pop() == 'd'	);
	Assert( cb4.reserve() == 8	);
	Assert( cb4.size() == 0     );

	Assert(	cb.pop() == 'a'		);
	Assert(	cb.pop() == 'b'		);
	Assert(	cb.pop() == 'c'		);
	Assert( cb.reserve() == 8	);
	Assert( cb.size() == 0      );

	return 0;
}

