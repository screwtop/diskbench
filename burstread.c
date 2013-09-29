
// Simple burst read disk benchmark
// cedwards 2005-09-27


#include <stdio.h>
#include <stdlib.h>	// For malloc().
#include <time.h>

#include "diskbench_timing.h"
#include "disksize.h"


int main(int argc, char* argv[])
{
	char* filename = argv[1];
	int i;
	int repetitions = 500;
	unsigned long sector_size = sectorsize(filename);
	int transfer_size_in_sectors = 128;	// This needs to be small enough to fit in the cache.
	long transfer_size_in_bytes = transfer_size_in_sectors * sector_size;
	double start_time, end_time;
	char * buf = malloc(transfer_size_in_bytes);
	FILE* fd = fopen(filename, "r");

	for (i = 0; i < repetitions; i++)
	{

		// Check the system time before doing the operation:
		start_time = seconds_since_epoch();

		// Note: fseek() location is in bytes.  To test burst read rate, read from the same location every time so that the data will be in the cache already.
		fseek(fd, 1, SEEK_SET);

		// Read from the same block
		fread(buf, transfer_size_in_bytes, 1, fd);

		// Record the time after finishing to determine elapsed time.
		end_time = seconds_since_epoch();

		printf("%f", (double)transfer_size_in_bytes / (end_time - start_time) / 1024.0 / 1024.0);
		printf("\n");
	}
	
	fclose(fd);
	return 0;
}

