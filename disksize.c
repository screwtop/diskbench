// $Id: disksize.c,v 1.2 2007/03/18 10:19:30 cedwards Exp $
// Simple function for determining the total size of a device (in bytes).  Used as a support function by the other diskbench utilities, which need a way to find out how big the device is (for sequential rate and random access time tests).

// Weird: the return value of this seems to get mangled when passed to the calling env.  Maybe a truncation problem.
// Ah, C automatically declares unknown functions as returning int.  You need to put a stub declaration in the other file ("extern long disksize(char* filename);").
// I guess this file should have a companion header file or something, right?


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>



// Unfortunately, the technique of seeking to the end of a device special file does not seem to work on Mac OS X (it returns 0 as the size).
// An alternative approach using ioctl() might work better.  This also permits determining the sector size of the device.
// However, this would only work if the requested file is actually a block device.
// TODO: maybe check and act accordingly, using the ioctl code below if it's a block device, and the seek-style code at the end if it's an ordinary file.

#if defined __APPLE__ && defined __MACH__
#include <ioctl.h>

off_t disksize (char* filename)
{
	long long sector_count, sector_size_in_bytes;
	off_t position;
	int f = 0;
	f = open(filename, O_RDONLY);
	if (f == -1)
	{
		fprintf(stderr, "disksize.c: disksize(): Error opening file \"%s\"!\n", filename);
		return 0;
	}

	// Query the number of sectors on the disk
	ioctl(f, DKIOCGETBLOCKCOUNT, &sector_count);

	unsigned int SectorSize = 0;
	// Query the size of each sector
	ioctl(f, DKIOCGETBLOCKSIZE, &sector_size_in_bytes);

	return sector_count * sector_size_in_bytes;
//	uint64_t DiskSize = (uint64_t)sector_count * (uint64_t)sector_size_in_bytes;
}


#else

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
#endif

