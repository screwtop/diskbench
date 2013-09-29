// Diskbench timing test

#include "diskbench_timing.h"
#include <stdio.h>
#include <features.h>

int main(int argc, char* argv[])
{
	double start_time, end_time;
	int i;

	start_time = seconds_since_epoch();

	for (i = 0; i < 100000; i++)
		printf("Hello, world!\n");

	end_time = seconds_since_epoch();

	fprintf(stderr, "\n\n%f s elapsed time\n", end_time - start_time);

	fprintf(stderr, "_POSIX_C_SOURCE = %li\n", _POSIX_C_SOURCE );
}

