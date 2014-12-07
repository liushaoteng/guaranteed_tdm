/*
 * highways.h
 *
 *  Created on: Jun 13, 2014
 *      Author: waterwood
 */

#ifndef PSEUDO_QUEUE_H_
#define PSEUDO_QUEUE_H_

#include "commonheader.h"
#include "networkdef.h"

#include "assert.h"
#include "peq_with_get_map.h"


class Pseudo_queue {
public:
   Pseudo_queue(int depth);
   virtual ~Pseudo_queue();
   void push();
   void pop();
   bool isEmpty();
   bool isFull();

   void release();

   int getToken();
   void refill();
   bool tailFlit();


   int currentPointer;

   int weight; // for queue arbitration purpose;
   int refill_slot;
   int weight_counter;

   int in_direction;
   int out_direction;


   int leftover;  // may be unuseful
   int valid;

   bool release_marker;
   int nextQueueToken;

   tlm::tlm_generic_payload* trans;

   Pseudo_queue (const Pseudo_queue & orig) {
       this->currentPointer = orig.currentPointer;
       this->weight = orig.weight;
       this->refill_slot = orig.refill_slot;
       this->weight_counter = orig.weight_counter;
       this->in_direction = orig.in_direction;
       this->out_direction = orig.out_direction;
       this->leftover = orig.leftover;
       this->valid = orig.valid;
       this->release_marker = orig.release_marker;
       this->nextQueueToken = orig.nextQueueToken;
       this->trans = orig.trans;
       this->_c_depth=orig._c_depth;
   }

private:
   int _c_depth;

};

#endif /* HIGHWAYS_H_ */
