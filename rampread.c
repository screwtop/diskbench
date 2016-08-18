
// Random disk I/O benchmark with variable transfer size (much like the classic ATTO benchmark).  This works much like randread.c, but will need to aggregate the results and print just a single averaged set of stats for each transfer size.
// Note: on Linux, disk cache effects may invalidate results.  In particular, if the random number generator uses the same seed each time, the same I/O pattern will be generated, resulting in potentially the entire test running from the disk cache in RAM!
// cedwards 2005-09-27

// To do:
//  - [Done] Figure out how to stop from trying to read beyond the end of the file/device!

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>	// For malloc()
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>	// for gettimeofday (used only for seeding the RNG)
#include <errno.h>

#include "disksize.h"
#include "diskbench_timing.h"


// TODO: should this be varied automatically according to the underlying hardware?  At least parameterised on the command line.
//#define SECTOR_SIZE 4096
//#define SECTOR_SIZE 512
// Now using a separate function to determine physical sector size.

// These are in sectors, not bytes:
// Hm, but maybe they should be in exponents of base 2, to make a for loop easier to write.
#define START_TRANSFER_SIZE_EXPONENT 0
#define END_TRANSFER_SIZE_EXPONENT 20
//#define START_TRANSFER_SIZE 64
//#define END_TRANFER_SIZE 32768

// See random (3)
#define MAX_RAND_INT 2147483648


int main(int argc, char* argv[])
{
	char* filename = argv[1];
	off_t disk_size_in_bytes = disksize(filename);
	int i;
	double start_time, end_time;
	unsigned long sector_size = sectorsize(filename);
	long disk_size_in_sectors = disk_size_in_bytes / sector_size;
	int transfer_size_in_sectors = 1 << START_TRANSFER_SIZE_EXPONENT;	// Probably doesn't make any sense to be anything other than 1.  Except maybe 64 or 128.
	long transfer_size_in_bytes = transfer_size_in_sectors * sector_size;
	// For rampread, we probably want to vary this according to the transfer size so that it takes a similar amount of time for each size.
	int transfer_size_exponent = 0;
	double time_limit_in_seconds = 5.0;
//	double time_limit_in_seconds = 15.0;	// per sector size iteration
	int repetitions = 0;

	struct timeval seed;

	// Scaling factor for the random numbers, to ensure that the sector chosen actually lies within the range of sectors on the disk.
	double block_scaling_factor = (double)MAX_RAND_INT / (double)disk_size_in_sectors;
	//fprintf(stderr, "block scaling factor = %f\n", block_scaling_factor);
	
	// Seed the random-number generator from current time to avoid repetitive I/O patterns that could be affected by cacheing.
	gettimeofday(&seed, NULL);
	srandom(seed.tv_usec);

//	char* buf = malloc(transfer_size_in_bytes);
	void* buf; // Data are read from disk into here.

	int fd = diskbench_open(filename);

	off_t sector = 0;
	double elapsed_time_in_seconds = 0;


	fprintf(stderr, "\nAbout to perform random access test on %s\n\n", filename);
	fprintf(stderr, "Disk size: %lli GiB\n", disk_size_in_bytes / 1024 / 1024 / 1024);
	fprintf(stderr, "           %lli MiB\n", disk_size_in_bytes / 1024 / 1024);
	fprintf(stderr, "           %lli kiB\n", disk_size_in_bytes / 1024);
	fprintf(stderr, "           %jd bytes\n", (intmax_t)disk_size_in_bytes);
	fprintf(stderr, "           %li %lu-byte sectors\n\n", disk_size_in_sectors, sector_size);
	

	// Loop through transfer sizes...
	for (transfer_size_exponent = START_TRANSFER_SIZE_EXPONENT; transfer_size_exponent <= END_TRANSFER_SIZE_EXPONENT; transfer_size_exponent++)
	{
		transfer_size_in_sectors = 1 << transfer_size_exponent;
		transfer_size_in_bytes = transfer_size_in_sectors * sector_size;
	//	buf = malloc(transfer_size_in_bytes);
		int memalign_status = posix_memalign(&buf, 512, transfer_size_in_bytes);
		if (memalign_status != 0) {
			perror("Error allocating aligned buffer memory.");
			exit(EXIT_FAILURE);
		}

	//	repetitions = 500;
	//	repetitions = 1 << (END_TRANSFER_SIZE_EXPONENT - transfer_size_exponent) + 0;
	//	repetitions = (int) round((1<<23) / (double)transfer_size_in_bytes) + 40;
	//	repetitions = 100;
		repetitions = 0;

	//	fprintf(stderr, "transfer size: %li sectors (%li bytes), repetitions = %li\n", transfer_size_in_sectors, transfer_size_in_bytes, repetitions);

		// Check the system time before doing all the reads:
		elapsed_time_in_seconds = 0.0;
		start_time = seconds_since_epoch();

		// ...performing a random pattern of reads of a particular size
		// TODO: maybe have this a while loop and exit after a certain amount of time has passed.
	//	for (i = 0; i < repetitions; i++)
		while (elapsed_time_in_seconds < time_limit_in_seconds)
		{
			sector = random() / block_scaling_factor;
			
			// Note: fseek() location is in bytes.
			lseek(fd, sector * sector_size, SEEK_SET);
			
			int read_result = read(fd, buf, transfer_size_in_bytes);

			// Oh, what if there's an error using O_DIRECT, but it occurs after the read(), not the open()?
			if (read_result < 0) {
				perror("Read error!");
				return -1;
			}

	//	}

			// Record the time after finishing all the reads to determine the elapsed time.
			// TODO: consider the computational time incurred by this?  I'm guessing it's probably negligible, but...
			end_time = seconds_since_epoch();

			elapsed_time_in_seconds = end_time - start_time;

			repetitions++;
		}

		// print statistics on the run for the current tranfer size:
//		printf("transfer size: %li sectors (%li bytes), repetitions = %li\t", transfer_size_in_sectors, transfer_size_in_bytes, repetitions);
		printf("%i sectors", transfer_size_in_sectors);
		printf("\t");
		printf("%li B", transfer_size_in_bytes);
		printf("\t");
		printf("%f ms", 1000.0 * elapsed_time_in_seconds);
		printf("\t");
		printf("%f MiB/s", (double)repetitions * (double)transfer_size_in_bytes / elapsed_time_in_seconds / 1024.0 / 1024.0);
		printf("\t");
		printf("%f IOPS", (double)repetitions / elapsed_time_in_seconds);
		printf("\n");

		free(buf);
	}

	close(fd);
	return 0;
}
