/*
 * top.cpp
 *
 *  Created on: Mar 21, 2012
 *      Author: lsteng
 */

#include "top.h"
using namespace sc_core;
namespace PPB {

  Top::~Top() {
  // delete dummy;
    for (int k=0; k<NUM_SUB_NETWORKS;k++)
      {
	delete dummy[k];
	for (int col = 0; col < COL; col++)
	  for (int row = 0; row < ROW; row++) {
            int self = col + row * COL;
          
            delete router[k][self];
	  }

    // TODO Auto-generated destructor stub
      }

    for (int col = 0; col < COL; col++)
	  for (int row = 0; row < ROW; row++) {
            int self = col + row * COL;
            delete resource[self];
	  }
}
  
 void Top::updateSlotTime() {
     Slot.increasePositive();
     Slot.increaseNegtive();

     updateSlot.notify(SLOTWIDTH, SC_NS);
 }

} /* namespace PPB */
