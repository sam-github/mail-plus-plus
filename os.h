//
// Os: a wrapper around O/S functionality.
//

#ifndef M_OS_H
#define M_OS_H

#include <unistd.h>
#include <iostream.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unix.h>
#include <pwd.h>

class MOs
{
public:
	static int IsaTty(int fd)
	{
		return isatty(fd);
	}

	static const char* UserName()
	{
		struct passwd* pw = getpwuid(getuid());
		if(!pw) {
			return "unknown";
		}
		return pw->pw_name;
	}
	static const char* Comment()
	{
		struct passwd* pw = getpwuid(getuid());

		if(!pw) {
			return "";
		}
		return pw->pw_comment;
	}

	static const char* HostName()
	{
		static char hostname[MAXHOSTNAMELEN + 1] = "";

		gethostname(hostname, sizeof(hostname));

		return hostname;
	}
};

#endif

