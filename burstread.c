
// Simple burst read disk benchmark
// cedwards 2005-09-27


#include <stdio.h>
#include <stdlib.h>	// For malloc().
#include <time.h>


#define SECTOR_SIZE 512


int main(int argc, char* argv[])
{
	char* filename = argv[1];
	int i;
	int repetitions = 500;
	int transfer_size_in_sectors = 128;	// This needs to be small enough to fit in the cache.
	long transfer_size_in_bytes = transfer_size_in_sectors * SECTOR_SIZE;
	struct timespec time0, time1;
	char * buf = malloc(transfer_size_in_bytes);
	FILE* fd = fopen(filename, "r");

	for (i = 0; i < repetitions; i++)
	{

		// Check the system time before doing the operation:
		clock_gettime(CLOCK_REALTIME, &time0);

		// Note: fseek() location is in bytes.  To test burst read rate, read from the same location every time so that the data will be in the cache already.
		fseek(fd, 1, SEEK_SET);

		// Read from the same block
		fread(buf, transfer_size_in_bytes, 1, fd);

		// Record the time after finishing to determine elapsed time.
		clock_gettime(CLOCK_REALTIME, &time1);

		printf("%f", (double)transfer_size_in_bytes / ((double)(time1.tv_sec - time0.tv_sec) + (double)(time1.tv_nsec - time0.tv_nsec) / 1000000000.0) / 1024.0 / 1024.0);
		printf("\n");
	}
	
	fclose(fd);
	return 0;
}

