// Return time since the system time reference epoch in seconds
// How much sub-second precision does this typically give, if we do the calculations using doubles?

#include <features.h>
// Yay, Catch-22: to find out if we're on a suitable POSIX system, we have to include <features.h>, which might not be available on Mac OS X.
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>

// For POSIX systems, this is just a wrapper around clock_gettime():

double seconds_since_epoch() {
	struct timespec timespec_now;

	// For disk benchmarking, times are generally long and we care primarily about elapsed real time.  However, the times might be affected by system clock adjustments, and I'm not certain of the precision on typical systems.
	clock_gettime(CLOCK_REALTIME, &timespec_now);
//	clock_gettime(CLOCK_MONOTONIC, &timespec_now);
	return (double)timespec_now.tv_sec + (double)timespec_now.tv_nsec / 1000000000.0;
}
#endif




// Implementation for Mac OS X:
#if defined __APPLE__ && defined __MACH__
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <stdint.h>

// Mac OS X lacks the clock_gettime() call, but has mach_absolute_time() instead, which should be just as good.

double seconds_since_epoch() {
	uint64_t time = mach_absolute_time();
	mach_timebase_info_data_t timeBase;
	(void)mach_timebase_info( &timeBase );
	return (double)timeBase.numer / (double)timeBase.denom / 1000000000.0;
}
#endif

