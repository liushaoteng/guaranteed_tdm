#include "networkdef.h"
//extern char *trans_type_name[]={"singleProbbe", "MultiProbe"};
//extern char *channel_status_name[]={"Idle", "Booked","Busy","Cancellation"};
#ifdef _INPUT_FROM_FILE
int ROW=2;
int COL=2;
#endif
//int COL_BITS =4;
char path[100];
int lifeTime;
double scale_factor;
SlotManager Slot;

