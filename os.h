//
// Os: a wrapper around O/S functionality.
//

#ifndef MOS_H
#define MOS_H

#include <unistd.h>
#include <iostream.h>
#include <sys/param.h>
#include <unix.h>

class MOs
{
public:
	static int IsaTty(int fd)
	{
		return isatty(fd);
	}

	static const char* UserName()
	{
		return getlogin();
	}
	static const char* HostName()
	{
		static char hostname[MAXHOSTNAMELEN + 1] = "";

		gethostname(hostname, sizeof(hostname));

		return hostname;
	}
};

#endif

