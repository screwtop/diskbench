
// Rewrite disk contents to cause bad sectors to be remapped by the drive, if possible.  Can also just be used on individual files.
// cedwards 2005-10-15
// NOTE: I'm not sure I've ever actually tested/used this!

#include <stdio.h>
#include <stdlib.h>	// For malloc()
#include <fcntl.h>	// for open()
#include <unistd.h>	// for lseek()
#include <stdint.h>	// for intmax_t
#include <time.h>

#include "disksize.h"


int main(int argc, char* argv[])
{
	char* filename = argv[1];

	// Determine size of file/device being tested:
	off_t disk_size_in_bytes = disksize(filename);
	unsigned long sector_size = sectorsize(filename);
	long disk_size_in_sectors = disk_size_in_bytes / sector_size;

	int transfer_size_in_sectors = 1;
	
	long transfer_size_in_bytes = transfer_size_in_sectors * sector_size;
	
	long sector = 0;
	
	off_t byte = 0;
	
	char * buf = malloc(transfer_size_in_bytes);

	
	int fd = open(filename, O_RDWR | O_DSYNC);
	
	for (sector = 0; sector < disk_size_in_sectors; sector++)
	{
		printf("\t%f", (double)sector / (double)disk_size_in_sectors);

		byte = sector * sector_size;
		
		printf("\t%jd\n", (intmax_t)byte);
					
		lseek(fd, byte, SEEK_SET);

		// Read the block at the current location:		
		read(fd, buf, transfer_size_in_bytes);
		
		lseek (fd,byte, SEEK_SET);
		
		// Write it back to the disk:
		write(fd, buf, transfer_size_in_bytes);
		
	}
	
	free(buf);
	close(fd);
	return 0;
}

