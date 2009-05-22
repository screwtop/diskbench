
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
// #include <sys/time.h>


#define SECTOR_SIZE 512

// See random (3)
#define MAX_RAND_INT 2147483648

extern off_t disksize(char* filename);


int main(int argc, char* argv[])
{
	char* filename = argv[1];
	off_t disk_size_in_bytes = disksize(filename);
	int i;
	struct timespec time0, time1;
	long disk_size_in_sectors = disk_size_in_bytes / SECTOR_SIZE;
	int transfer_size_in_sectors = 1;	// Probably doesn't make any sense to be anything other than 1.  Except maybe 64 or 128.
	long transfer_size_in_bytes = transfer_size_in_sectors * SECTOR_SIZE;
	int repetitions = 10000;

	struct timeval seed;

	// Scaling factor for the random numbers, to ensure that the sector chosen actually lies within the range of sectors on the disk.
	double block_scaling_factor = (double)MAX_RAND_INT / (double)disk_size_in_sectors;
	//fprintf(stderr, "block scaling factor = %f\n", block_scaling_factor);
	
	// Warning: seeding with 0 will result in the same I/O pattern, potentially giving bogus results on multiple runs.
	//srandom(0);
	gettimeofday(&seed, NULL);
	srandom(seed.tv_usec);

	char* buf = malloc(transfer_size_in_bytes);

	int fd = open(filename, O_RDONLY);
//	int fd = open(filename, O_RDONLY | O_DIRECT);
//	int fd = open(filename, O_RDONLY | O_DIRECT | O_SYNC);
	off_t sector = 0;
	double elapsed_time_in_seconds = 0;
	
	fprintf(stderr, "\nAbout to perform random access test on %s\n\n", filename);
	fprintf(stderr, "Disk size: %li GiB\n", disk_size_in_bytes / 1024 / 1024 / 1024);
	fprintf(stderr, "           %li MiB\n", disk_size_in_bytes / 1024 / 1024);
	fprintf(stderr, "           %li kiB\n", disk_size_in_bytes / 1024);
	fprintf(stderr, "           %lli bytes\n", disk_size_in_bytes);
	fprintf(stderr, "           %li %li-byte sectors\n\n", disk_size_in_sectors, SECTOR_SIZE);
	
	for (i = 0; i < repetitions; i++)
	{
		sector = random() / block_scaling_factor;
		
		// Sector being sought:
		printf("%li", sector);

		// Byte being sought:
		printf("\t%lli", sector * (long)SECTOR_SIZE);

		// Location as a fraction of the entire drive space:
		printf("\t%f", (double)sector / (double)disk_size_in_sectors);

		
		// Check the system time before doing the operation:
		clock_gettime(CLOCK_REALTIME, &time0);
		
		// Note: fseek() location is in bytes.
		lseek(fd, sector * (long)SECTOR_SIZE, SEEK_SET);
		
		// Does it make any difference if we only read one byte, rather than the whole block?
		read(fd, buf, transfer_size_in_bytes);

		// Record the time after finishing to determine elapsed time.
		clock_gettime(CLOCK_REALTIME, &time1);
		
		elapsed_time_in_seconds = (double)(time1.tv_sec - time0.tv_sec) + (double)(time1.tv_nsec - time0.tv_nsec) / 1000000000.0;
		
//		printf("; elapsed = %i s", time1.tv_sec - time0.tv_sec);
		printf("\t");
		// Print elapsed time in milliseconds:
		printf("%f", 1000 * ((double)(time1.tv_sec - time0.tv_sec) + (double)(time1.tv_nsec - time0.tv_nsec) / 1000000000));

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

