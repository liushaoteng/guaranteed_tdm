#!/usr/bin/perl
open (OUTFILE,">../src/core_of_single_router/configuration.h")||die ("can not open $!");

print(OUTFILE
"#define COL $ARGV[0]
#define ROW $ARGV[1]
#define NUM_NODES $ARGV[2]
#define NUM_DIRECTIONS $ARGV[3]
#define CHANNEL_FANOUTS $ARGV[4]
#define TOTAL_NUM_CHANNELS  $ARGV[5]
#define RESOURCE_CHANNEL_NUM $ARGV[6]  //= NUM_SUB_NETWORKS*CHANNEL_PER_DIRECTION
#define CHANNEL_PER_DIRECTION $ARGV[7]
#define COL_BITS 4 //max col=2^Col_bits
#define NUM_SUB_NETWORKS $ARGV[8]
#define MAX_TIME $ARGV[9]
#define SLOTDEPTH $ARGV[10]
#define SLOTWIDTH 1
#define RETRYTIMES 5
#define HIGHWAYDEPTH $ARGV[11]
#define HIGHWAYOFEACHDIR $ARGV[12]
\n");

close (OUTFILE);
exit;
