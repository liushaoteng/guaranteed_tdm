/*
 * highways.cpp
 *
 *  Created on: Jun 13, 2014
 *      Author: waterwood
 */

#include "Pseudo_queue.h"
using namespace PPB;

Pseudo_queue::Pseudo_queue(int depth):_c_depth(depth) {
    // TODO Auto-generated constructor stub
//    currentPointer=0;
//
//    refill_slot =-1;
//    leftover=_c_depth;  //leftover may be not useful, we will see
//    weight=0;
//    weight_counter =0;
//
//    valid=1;
//
//    nextQueueToken =0;
//
//    in_direction = Nodrctn;
//    out_direction = Nodrctn;
//    trans=NULL;
//    release_marker =false;

    release();

}
void Pseudo_queue::release(){
    currentPointer=0;

       refill_slot =-1;
       leftover=_c_depth;  //leftover buffer may be not used, we will see
       weight=0;
       weight_counter =0;

       valid=1;

       nextQueueToken =0;

       in_direction = Nodrctn;
       out_direction = Nodrctn;
       trans=NULL;
       release_marker =false;
}


void Pseudo_queue::push(){
    currentPointer++;


    assert (currentPointer <= _c_depth || refill_slot == Slot.readPositive());

}

void Pseudo_queue::pop() {
    currentPointer--;
    if (weight_counter >0)
        weight_counter--;

    if (refill_slot != Slot.readPositive())
    nextQueueToken--;

    assert (currentPointer >=0);

}

bool Pseudo_queue::isEmpty() {
    return (currentPointer <=0)? 1:0;

}

bool Pseudo_queue::isFull () {

    return (currentPointer==_c_depth)?1:0;
}

int Pseudo_queue::getToken() {

    return _c_depth-currentPointer;
}


void Pseudo_queue::refill(){
    weight_counter = weight;

}

bool Pseudo_queue::tailFlit(){
    return (currentPointer ==1 && release_marker )? 1:0;
}


Pseudo_queue::~Pseudo_queue() {
    // TODO Auto-generated destructor stub
}

