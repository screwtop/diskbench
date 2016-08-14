// $Id: disksize.c,v 1.2 2007/03/18 10:19:30 cedwards Exp $
// Functions for determining the physical sector size and total size of a device.
// Used as support functions by the other diskbench utilities, which need to know these things (for sequential rate and random access time tests).
// disksize() returns the size of the file/device in bytes (as an off_t).
// sectorsize() returns the size of a physical sector ("block").

// Weird: the return value of this seems to get mangled when passed to the calling env.  Maybe a truncation problem.
// Ah, C automatically declares unknown functions as returning int.  You need to put a stub declaration in the other file ("extern long disksize(char* filename);").
// TODO: I guess this file should have a companion header file or something, right?

// TODO: consider using ioctl BLKSSZGET on Linux to determine logical block size?  No, apparently I already tried that and libblkid proved more reliable.

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>



// Unfortunately, the technique of seeking to the end of a device special file does not seem to work on Mac OS X (it returns 0 as the size).
// An alternative approach using ioctl() might work better.  This also permits determining the sector size of the device.
// However, this would only work if the requested file is actually a block device.
// TODO: maybe check and act accordingly, using the ioctl code below if it's a block device, and the seek-style code at the end if it's an ordinary file.

#if defined __APPLE__ && defined __MACH__
#include <sys/ioctl.h>
#include <sys/disk.h>

off_t disksize (char* filename)
{
	unsigned long long sector_count;
	uint32_t sector_size_in_bytes;
	int f = 0;
	f = open(filename, O_RDONLY);
	if (f == -1)
	{
		fprintf(stderr, "disksize.c: disksize(): Error opening file \"%s\"!\n", filename);
		return 0;
	}

	// Query the number of sectors on the disk
	ioctl(f, DKIOCGETBLOCKCOUNT, &sector_count);
	
	// Query the size of each sector
	ioctl(f, DKIOCGETPHYSICALBLOCKSIZE, &sector_size_in_bytes);

	return (uintmax_t)sector_count * (uintmax_t)sector_size_in_bytes;
//	uint64_t DiskSize = (uint64_t)sector_count * (uint64_t)sector_size_in_bytes;
}

size_t sectorsize (char* filename)
{
	uint32_t sector_size_in_bytes;
	int f = 0;
	f = open(filename, O_RDONLY);
	if (f == -1) {
		fprintf(stderr, "disksize.c: disksize(): Error opening file \"%s\"!\n", filename);
		return 0;
	} else {
		int status = ioctl(f, DKIOCGETPHYSICALBLOCKSIZE, &sector_size_in_bytes);
		return status == 0 ? sector_size_in_bytes : 0;
	}
}

#endif /* Mac OS X */


#if defined __linux__ || defined __NetBSD__

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

#endif /* Linux/NetBSD */


#if defined __linux__

// Linux-specific implementation that uses libblkid.
// (Linux-only?) function for determining the block size of the medium.  Only applicable for block device special files, not ordinary files?
// BLKSSZGET gives the logical sector size (probably always 512 bytes for a hard-disk-like device)
// BLKBSZGET gives the physical sector size (increasingly 4096 bytes for DASDs, 2048 bytes for optical disc devices)
// ...only it seems to give 4096 bytes when it shouldn't.  Is it maybe returning the filesystem block size, not the physical or logical disk block size?!
// fdisk gets it right; how does it do that, then?
// fdisk -l /dev/sda (SSD): 512 B logical, 4096 B physical, 4096 B min I/O, 4096 B optimal I/O
// fdisk -l /dev/sdb (HDD): 512, 512, 512, 512
// fdisk -l /dev/sdc (HDD): 512, 512, 512, 512
// Ah, OK, fdisk uses libblkid.  Let's try that then...

#include <sys/mount.h>
#include <sys/ioctl.h>
#include <blkid/blkid.h>

size_t sectorsize (char* filename)
{
	unsigned long sector_size = 0;
	int fd = 0;
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "disksize.c: sectorsize(): Error opening file \"%s\"!\n", filename);
		return 0;
	} else {
		// Use libblkid to probe the device's physical sector size...
		blkid_probe probe;
		probe = blkid_new_probe();
		if (probe && blkid_probe_set_device(probe, fd, 0, 0) == 0) {
			blkid_topology tp = blkid_probe_get_topology(probe);
			if (tp) {
				sector_size = blkid_topology_get_physical_sector_size(tp);
			}
		}
		blkid_free_probe(probe);

		return sector_size;
	}
}

#endif

#if defined __NetBSD__

size_t sectorsize (char* filename)
{
	// Dummy implementation for now...
	return 512;
}

#endif
