
// Simple sequential read disk benchmark
// cedwards 2005-09-27

// To do:
//  - Stop benchmark from trying to read beyond end of file/disk (causes increasing STR approaching end of drive).
//  - Implement proper zone sampling scheme based on zone midpoints and proportion of zone to be tested.

/*

   Divide the disk up into Z zones (where Z would be specified by the user).
   Perform one sequential read test for each zone, reading a quantity of data not greater than the size of the zone (to avoid reading beyond EOF).  Maybe specified as a proportion of the zone size.

*/

#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>	// For malloc()
#include <fcntl.h>	// for open()
#include <unistd.h>	// for lseek()
#include <time.h>
#include <errno.h>

#include "diskbench_timing.h"


// NOTE: This won't always be true!  Storage industry is currently (2010) preparing to move to 4 kiB sectors.
#define SECTOR_SIZE 4096L
//#define SECTOR_SIZE 512L

extern off_t disksize(char* filename);


int main(int argc, char* argv[])
{
	char* filename = argv[1];

	// Determine size of file/device being tested:
	off_t disk_size_in_bytes = disksize(filename);

	long disk_size_in_sectors = disk_size_in_bytes / SECTOR_SIZE;

	long zone_count = 256;	// To be user-specified.
	long zone_size_in_sectors = disk_size_in_sectors / zone_count;
	long zone_size_in_bytes = zone_size_in_sectors * SECTOR_SIZE;

//	float zone_sampling_factor = 0.01;
	// Maybe it makes more sense to specify what proportion of the disk you want to check, irrespective of the number of zones.
	float disk_sampling_factor = 0.0001;
	
	int transfer_size_in_sectors = 32768;
//	int transfer_size_in_sectors = 4096; // To match hdparm's 2 MiB, in the hope it'll align nicely for O_DIRECT.
//	transfer_size_in_sectors = transfer_size_in_sectors <= zone_size_in_sectors ? transfer_size_in_sectors : zone_size_in_sectors;	// To be user-specified.  Needs to be at least as big as the drive buffer cache, but by how wuch, ideally?  Should also be less than the zone size.
//	int transfer_size_in_sectors = zone_size_in_sectors * zone_sampling_factor;	// I think there's also a limit to how much can be transferred in a single request, so this should also be truncated to whatever that limit happens to be.
//	int transfer_size_in_sectors = disk_size_in_sectors * disk_sampling_factor <= zone_size_in_sectors ? disk_size_in_sectors * disk_sampling_factor : zone_size_in_sectors;	// Newer sampling scheme, based on proportion of entire disk to be checked.  This should be forced to be less than the zone size.
	// Or maybe it should be the closest power of 2 in sectors that's less than the zone size...
	
	long transfer_size_in_bytes = transfer_size_in_sectors * SECTOR_SIZE;
	
	double start_time, end_time;

	int zone = 0;
	
	char * buf = malloc(transfer_size_in_bytes);

	fprintf(stderr, "\nAbout to perform sequential read test on %s\n\n", filename);
	fprintf(stderr, "Disk size: %li GiB\n", disk_size_in_bytes / 1024 / 1024 / 1024);
	fprintf(stderr, "           %li MiB\n", disk_size_in_bytes / 1024 / 1024);
	fprintf(stderr, "           %li kiB\n", disk_size_in_bytes / 1024);
	fprintf(stderr, "           %lli bytes\n", disk_size_in_bytes);
	fprintf(stderr, "           %li %li-byte sectors\n\n", disk_size_in_sectors, SECTOR_SIZE);

	fprintf(stderr, "Test parameters:\n");
	fprintf(stderr, "  * Number of zones: %li\n", zone_count);
	fprintf(stderr, "  * Transfer size: %li sectors\n", transfer_size_in_sectors);
	fprintf(stderr, "                   %li bytes\n", transfer_size_in_bytes);
	fprintf(stderr, "                   %li kiB\n", transfer_size_in_bytes / 1024);
	fprintf(stderr, "                   %li MiB\n", transfer_size_in_bytes / 1024 / 1024);
	fprintf(stderr, "\n");

	// Heading for output:
	fprintf(stderr, "zone(sector)\tzone(decimal)\trate(MiB/s)\n");
	
	long long zone_midpoint_sector = 0;
	
	int read_result = 0, return_code = 0;
	
	// Also, perhaps O_LARGEFILE?
//	int fd = open(filename, O_RDONLY | O_DIRECT | O_LARGEFILE);
//	int fd = open(filename, O_RDONLY | O_DIRECT | O_SYNC);
//	int fd = open(filename, O_RDONLY | O_DIRECT);
	int fd = open(filename, O_RDONLY);

        // NOTE: consider sys_fadvise64_64() here, perhaps with FADV_NOREUSE.

	// TODO: Check that the file was opened successfully, and bail out if not.
	if (fd == -1) {return_code = errno; perror("Error open()ing file"); return return_code;}

//	sys_fadvise64_64();
	
	for (zone = 0; zone < zone_count; zone++)
	{
		zone_midpoint_sector = (double)zone * (double)zone_size_in_sectors + (double)zone_size_in_sectors / 2.0;
		
		// Print midpoint sector of current test transfer:
		printf("%li", zone_midpoint_sector);
		// Might make ploticus output easier if we just report the location as fraction of entire drive.
		printf("\t%f", (double)zone_midpoint_sector / (double)disk_size_in_sectors);
			
		// Check the system time before doing the operation:
		start_time = seconds_since_epoch();
		
		// Note: fseek() location is in bytes.  How do we do this on 32-bit systems?
		lseek(fd, (zone_midpoint_sector - transfer_size_in_sectors / 2.0) * SECTOR_SIZE, SEEK_SET);
	//	perror("lseek() status");
		
		// Read a big chunk of data of a known size in one I/O:
		// NOTE: Linux has alignment restrictions on direct I/O.  These may be highly variable, and difficult to determine/comply with.  TODO: investigate, try.  512 B seems to be common.  Does the file also have to be so aligned?
		read_result = read(fd, buf, transfer_size_in_bytes);
		//fread(buf, transfer_size_in_bytes, 1, fd);

	//	fprintf(stderr, "\n%i bytes read\n", read_result);

		if (read_result < 0) {return_code = errno; perror("Error read()ing disk/file"); fprintf(stderr, "\n%i bytes reported as read\n", read_result); return return_code;}

		// Record the time after finishing to determine elapsed time.
		end_time = seconds_since_epoch();
		
		// Average transfer rate for the transfer is transfer size / elapsed time.  MiB/s is probably an appropriate unit.
		printf("\t%f", (double)transfer_size_in_bytes / (end_time - start_time) / 1024.0 / 1024.0);
//		fprintf(stderr, "\t[%f % done]", (double)zone / (double)zone_count * 100.0);
		printf("\n");
	}
	
	free(buf);
	close(fd);
	return 0;
}

