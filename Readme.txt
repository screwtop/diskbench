This is diskbench, a collection of simple low-level disk benchmarks implemented as command-line C programs.

NOTE: this is currently a work-in-progress and should be considered alpha-quality.  Don't expect it to work, and use at your 
own risk!


The Utilities:

  seqread    Performs a series of large sequential read operations across the device, reporting the transfer rate for each.
             This benchmark may be more relevant for applications dealing with individual large files, such as video and image manipulation.

  randread   Performs a series of random reads of one block in size, reporting the time per operation in milliseconds, and the transfer rate.
             Random I/O operations occur more frequently in typical database applications, typical defragmentation routines, and applications involving large numbers of small files, especially under heavy concurrency.
             Because the operation includes the retrieval of the block, the time reported will be slightly higher than the actual access time (but generally not by very much).

  burstread  Repeatedly re-reads the same chunk of data from the device, so that it will be retrieved from the disk drive cache rather than from the physical medium.
             Burst (cache) reads occur when the same data are retrieved repeatedly by the system, especially when subsequent reads happen soon after the initial read.


The commands are intended to operate on entire devices, so the argument will usually be the special device file for the 
disk you wish to test (e.g. '/dev/sda' or '/dev/rsd0d').  However, you can also test individual partitions (e.g. 
'/dev/sda1') or individual files ('~/Video/Wedding.avi'), in which case the apparent performance will probably be much 
greater[1], and not comparable with other systems.

If you have no idea what device filename to use, try one of these:
	Linux     /dev/sda OR /dev/hda
	NetBSD    /dev/rwd0d OR /dev/rsd0d
	Mac OS X  /dev/???
	MINIX 3   /dev/c0d0p0s0???

You will probably have to run these commands as root in order to access the raw disk devices.


Also included is a simple program 'rewrite', which will read and write a disk/partition/file, leaving it unchanged, but 
potentially causing the disk drive to detect and re-map any disk sectors that may be in the process of going bad.  This is 
no substitute for a backup, but may be useful anyway.  Because it does one sector at a time, expect it to be extremely 
slow.  For cases in which sectors have already become unreadable, try ddrescue (another fine GNU utility from the Free 
Software Foundation).


[1] You can sacrifice capacity for performance when partitioning a drive.  The data at the outside edge of a disk will 
generally experience faster sequential data rates (perhaps 2x the speed of the inside edge), so using a small slice of 
the disk will effectively give a small, fast disk.  This will also benefit access times and random I/O performance, since 
the read/write head need only move within a much smaller region of the disk.


Compatibility:

My intention is to have the diskbench programs compile and run on NetBSD, Linux, Cygwin/Windows, Mac OS X, and MINIX 3.  Other POSIX compliant systems should be easy to support (they may even just work).

Linux kernel version 2.6.10 or later are necessary for O_DIRECT support.

Currently diskbench should work on NetBSD, Linux and Cygwin systems, since I have ready access to these and can test them.

