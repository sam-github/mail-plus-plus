
#include <mail++/fire.h>


int main(int argc, char* argv[])
{
	for(int i = 1; argv[i]; i++)
	{
		FIRE_FILEINFO finfo;

		int err = FileRecognition(argv[i], &finfo, 0);

		printf("%s: err: %d  type: %d\n", argv[i], err, finfo.filetype);
	}
	return 0;
}

