// Simple function for determining the total size of a device.  Used as a support function by the other diskbench utilities, which need a way to find out how big the device is (for sequential rate and random access time tests).

#include <unistd.h>

off_t disksize (char* filename);
size_t sectorsize (char* filename);
int diskbench_open(const char* filename);
