
// Simple random disk I/O benchmark
// Note: on Linux, disk cache effects may invalidate results.  In particular, if the random number generator uses the same seed each time, the same I/O pattern will be generated, resulting in potentially the entire test running from the disk cache in RAM!
// cedwards 2005-09-27

// To do:
//  - [Done] Figure out how to stop from trying to read beyond the end of the file/device!

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>	// For malloc()
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>	// for gettimeofday (used only for seeding the RNG)

#include "diskbench_timing.h"
#include "disksize.h"

// See random (3)
#define MAX_RAND_INT 2147483648

//extern off_t disksize(char* filename);


int main(int argc, char* argv[])
{
	char* filename = argv[1];
	off_t disk_size_in_bytes = disksize(filename);
	int i;
	double start_time, end_time;
	unsigned long sector_size = sectorsize(filename);
	long disk_size_in_sectors = disk_size_in_bytes / sector_size;
	int transfer_size_in_sectors = 1;	// Probably doesn't make any sense to be anything other than 1.  Except maybe 64 or 128.
	long transfer_size_in_bytes = transfer_size_in_sectors * sector_size;
	int repetitions = 10000;

	struct timeval seed;

	// Scaling factor for the random numbers, to ensure that the sector chosen actually lies within the range of sectors on the disk.
	double block_scaling_factor = (double)MAX_RAND_INT / (double)disk_size_in_sectors;
	//fprintf(stderr, "block scaling factor = %f\n", block_scaling_factor);

	// Seed the RNG from the current time to avoid repetitive I/O patterns (and possible cache effects) across multiple runs:
	gettimeofday(&seed, NULL);
	srandom(seed.tv_usec);

//	char* buf = malloc(transfer_size_in_bytes);
	void* buf;
	int memalign_status = posix_memalign(&buf, 512, transfer_size_in_bytes);
	if (memalign_status != 0) {
		perror("Error allocating aligned buffer memory.");
		exit(EXIT_FAILURE);
	}


	int fd = diskbench_open(filename);

	off_t sector = 0;
	double elapsed_time_in_seconds = 0;
	
	fprintf(stderr, "\nAbout to perform random access test on %s\n\n", filename);
	fprintf(stderr, "Disk size: %li GiB\n", disk_size_in_bytes / 1024 / 1024 / 1024);
	fprintf(stderr, "           %li MiB\n", disk_size_in_bytes / 1024 / 1024);
	fprintf(stderr, "           %li kiB\n", disk_size_in_bytes / 1024);
	fprintf(stderr, "           %lli bytes\n", (long long int)disk_size_in_bytes);
	fprintf(stderr, "           %li %lu-byte sectors\n\n", disk_size_in_sectors, sector_size);
	
	for (i = 0; i < repetitions; i++)
	{
		sector = random() / block_scaling_factor;
		
		// Sector being sought:
		printf("%li", sector);

		// Byte being sought:
		printf("\t%lli", sector * (long long int)sector_size);

		// Location as a fraction of the entire drive space:
		printf("\t%f", (double)sector / (double)disk_size_in_sectors);

		
		// Check the system time before doing the operation:
		start_time = seconds_since_epoch();
		
		// Note: fseek() location is in bytes.
		lseek(fd, sector * (long)sector_size, SEEK_SET);
		
		// Does it make any difference if we only read one byte, rather than the whole block?
		read(fd, buf, transfer_size_in_bytes);

		// Record the time after finishing to determine elapsed time.
		end_time = seconds_since_epoch();
	
		elapsed_time_in_seconds = end_time - start_time;
		
//		printf("; elapsed = %i s", time1.tv_sec - time0.tv_sec);
		printf("\t");
		// Print elapsed time in milliseconds:
		printf("%f", elapsed_time_in_seconds * 1000.0);
	//	printf("%f", 1000 * ((double)(time1.tv_sec - time0.tv_sec) + (double)(time1.tv_nsec - time0.tv_nsec) / 1000000000));

		// Print throughput in MiB/s:
		printf("\t");
		printf("%f", (double)transfer_size_in_bytes / elapsed_time_in_seconds / 1024.0 / 1024.0);
		printf("\n");
		//printf("; time=%i, %i\n", (time1->tv_sec) - (time0->tv_sec), (time1->tv_nsec) - (time0->tv_nsec));
	}
	
	free(buf);
	close(fd);
	return 0;
}

