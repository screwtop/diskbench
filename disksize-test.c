// $Id: disksize-test.c,v 1.2 2007/03/18 10:19:30 cedwards Exp $
// Test program for the disksize function.  Give it a special device filename as an argument and it will tell you how big it is.

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>


#define SECTOR_SIZE 512L

extern off_t disksize(char* filename);

int main(int argc, char* argv[])
{
	// Initial testing:
/*
	int f = open(argv[1], O_RDONLY);
	printf("file opened OK\n");
	off_t position = lseek(f, 0L, SEEK_END);
	//off_t position = ftell(f);
	close(f);
	printf("sizeof(off_t) = %i\n", sizeof(off_t));
	printf("size of %s is %lli bytes = %lli sectors = %lli MiB = %lli GiB\n", argv[1], position, position / SECTOR_SIZE, position / 1024 / 1024, position / 1024 / 1024 / 1024);
*/


	off_t device_length_in_bytes = disksize(argv[1]);

	printf("%s is %jd bytes\n", argv[1], (intmax_t)device_length_in_bytes);
	printf("%s is %jd sectors\n", argv[1], (intmax_t)device_length_in_bytes / SECTOR_SIZE);
	printf("%s is ~%jd MiB\n", argv[1], (intmax_t)disksize(argv[1]) / 1024 / 1024);
	printf("%s is ~%jd GiB\n", argv[1], (intmax_t)device_length_in_bytes / 1024 / 1024 / 1024);

//	printf("test: %li\n", disksize(argv[1]));
//	long disk_size_in_bytes = (long)disksize(argv[1]);

//	printf("sizeof(disk_size_in_bytes) = %li\n", sizeof(disk_size_in_bytes));

//	long disk_size_in_sectors = disk_size_in_bytes / SECTOR_SIZE;

//	fprintf(stderr, "%s is %li bytes (%li sectors) long\n", argv[1], disk_size_in_bytes, disk_size_in_sectors);
	return 0;
}

