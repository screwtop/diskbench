// $Id: disksize.c,v 1.2 2007/03/18 10:19:30 cedwards Exp $
// Simple function for determining the total size of a device.  Used as a support function by the other diskbench utilities, which need a way to find out how big the device is (for sequential rate and random access time tests).

// Weird: the return value of this seems to get mangled when passed to the calling env.  Maybe a truncation problem.
// Ah, C automatically declares unknown functions as returning int.  You need to put a stub declaration in the other file ("extern long disksize(char* filename);").
// I guess this file should have a companion header file or something, right?


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

off_t disksize (char* filename)
{
	off_t position;
//	fprintf(stderr, "disksize: filename=\"%s\"\n", filename);
	int f = 0;
	f = open(filename, O_RDONLY);
	if (f == -1)
	{
		fprintf(stderr, "disksize.c: disksize(): Error opening file \"%s\"!\n", filename);
		return 0;
	}
	position = lseek(f, 0L, SEEK_END);
	// Just for debugging!
//	fprintf(stderr, "disksize: length=%lli bytes\n", position);
	close(f);
	return position;
}

