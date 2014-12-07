/*
 * Highways.cpp
 *
 *  Created on: Jun 13, 2014
 *      Author: waterwood
 */

#include "Highways.h"
using namespace std;
using namespace PPB;
using namespace tlm;

Highways::Highways(int numOfQueues, int depth, int numEachInDirect, int numEachOutDirect, string name):_numOfQueues(numOfQueues),
        _depth(depth),_numEachInDirect(numEachInDirect),_numEachOutDirect(numEachOutDirect), _name(name){
    //  cout<< " lala1 " <<endl;
    // Queue_repository.resize(numOfQueues,_depth);
    for(int i=0; i<numOfQueues; i++) {
        Pseudo_queue  tempQ(_depth);
        //     cout<< " lala2 " <<endl;
        Queue_repository.push_back(tempQ);
      //  cout << " queue " <<i << " depth is "  << Queue_repository[i].getToken();

    }
    _numDirections=NUM_DIRECTIONS;
    numOfFreeQueue.resize(_numDirections,_numEachInDirect);
    Request_queue.resize(_numDirections, NULL);
    nextRoundPointer=0;

    _input_arb.resize(_numDirections);
    _output_arb.resize(_numDirections);

    for (int i = 0; i < _numDirections; ++i) {
        _input_arb[i] = Arbiter::NewArbiter("higway_in","round_robin",_numEachInDirect);
        _output_arb[i] =  Arbiter::NewArbiter("higway_in","round_robin",_numDirections );
    }

    // TODO Auto-generated constructor stub

}


Highways::Highways(int numOfQueues, int depth, string name, int numDirections ):_numOfQueues(numOfQueues),
        _depth(depth),_numEachInDirect(numOfQueues),_numEachOutDirect(0), _name(name){
    _numDirections = numDirections;

    for(int i=0; i<numOfQueues; i++) {
        Pseudo_queue tempQ(_depth);
        Queue_repository.push_back(tempQ);
      //  cout << " queue " <<i << " depth is "  << Queue_repository[i].getToken();

    }
    numOfFreeQueue.resize(_numEachInDirect,_numEachInDirect);
    Request_queue.resize(1, NULL);
    nextRoundPointer=0;

    _input_arb.resize(1);
    _output_arb.resize(1);

    for (int i = 0; i < _numDirections; ++i) {
        _input_arb[i] = Arbiter::NewArbiter("higway_in","round_robin",_numEachInDirect);
        _output_arb[i] =  Arbiter::NewArbiter("higway_in","round_robin",_numDirections );
    }

    // TODO Auto-generated constructor stub

}


//void Highways::addRequest(tlm_generic_payload* trans, int in){  // this function is depreciated, since we do not use flexible queues.
//
//
//    Request_queue [in] = trans;
//
//}


//
//void Highways::allocate () {
//
//    int currentPointer;
//
//    int tempPointer;
//
//    Probe_extension *extension;
//
//
//    /** analyze the incoming transactions*/
//
//    for (int i=0; i< NUM_DIRECTIONS; i++) {
//        currentPointer = (nextRoundPointer+i)%NUM_DIRECTIONS;
//
//        if (Request_queue[currentPointer] != NULL) {
//            if (numOfFreeQueue >0) {
//
//                for (int j=0; j <_numOfQueues;j++){
//                    if (Queue_repository[j].valid) {
//                        numOfFreeQueue--;
//                        Queue_repository[j].valid=0;
//                        Queue_repository[j].in_direction = i;
//                        Queue_repository[j].trans = Request_queue[currentPointer];
//                        Queue_repository[j].refill_slot = SlotManager::readPositive();
//                        Queue_repository[j].trans->get_extension(extension);
//
//                        Queue_repository[j].weight = extension->multiChannelProbe;
//
//
//                        Queue_index.insert (pair <tlm_generic_payload*, int> (Request_queue[currentPointer],j) );
//
//
//                    }
//                }
//                tempPointer =currentPointer;
//            }
//        }
//
//    }
//    nextRoundPointer = (tempPointer+1)%NUM_DIRECTIONS;
//}


bool Highways::allocate(tlm_generic_payload* trans, int in, int slot) {
     Probe_extension *extension;
     trans->get_extension(extension);
    if (numOfFreeQueue[in] >0) {

        for (int j=in*_numEachInDirect; j <(in+1)*_numEachInDirect;j++){
            if (Queue_repository[j].valid) {
                numOfFreeQueue[in]--;
                Queue_repository[j].valid=0;
                Queue_repository[j].in_direction = in;
                Queue_repository[j].trans = trans;
                if (slot<0)
                    Queue_repository[j].refill_slot = SlotManager::readnegtive();
                else
                    Queue_repository[j].refill_slot = slot;
                //trans->get_extension(extension);

                //Queue_repository[j].weight = extension->multiChannelProbe;             //reserved for future multi-slot probe uses


                Queue_index.insert (pair <tlm_generic_payload*, int> (trans,j) );
#ifndef CONCISE
                cout <<_name <<  j <<" is allocated to tran " <<hex << setfill('0') << setw(4)<<trans->get_address() << " source slot " <<  extension->src_slotId  << " refill slot is "<< Queue_repository[j].refill_slot<< endl;
#endif
                //extension->haveQueue = true;
                return true;

            }
        }

    }

    //extension->haveQueue =false;


    return false;
}

void Highways::release(tlm_generic_payload * trans){

    int Q_ID = Queue_index.find(trans)->second;
   // cout << " Q_ID is " <<Q_ID << "test queue is "<<numOfFreeQueue[0] << endl;
    Queue_index.erase(trans);
    numOfFreeQueue[Q_ID/_numEachInDirect]++;
    Queue_repository[Q_ID].release();

//#ifndef CONCISE
//    cout <<_name << " Q_ID is " <<Q_ID << "free queue is "<<numOfFreeQueue[0] << " _num" << endl;
//#endif

}

void Highways::release(){

    //int Q_ID = Queue_index.find(trans)->second;
    for (int i=0; i < _numOfQueues; i++) {

        if (Queue_repository[i].isEmpty() && Queue_repository[i].release_marker) {

            Queue_index.erase(Queue_repository[i].trans);
            numOfFreeQueue[i/_numEachInDirect]++;
            Queue_repository[i].release();
        }
    }



}

bool Highways::isAvailable(int in ) {
#ifndef CONCISE
    cout <<_name <<  " number of Queue is " << numOfFreeQueue[in] << " direction " << in <<endl;
#endif
    return (numOfFreeQueue[in] >0)?1:0;
}



void Highways::configure(tlm_generic_payload* trans, int out){
    int Q_ID = Queue_index.find(trans)->second;
#ifndef CONCISE
    cout << trans->get_address() << " configure " << Q_ID << endl;
#endif
    assert (Queue_index.find(trans)!=Queue_index.end());

    Queue_repository[Q_ID].out_direction = out;
    Queue_repository[Q_ID].nextQueueToken = HIGHWAYDEPTH;

}

Pseudo_queue* Highways::getTheQueue(tlm::tlm_generic_payload* trans) {
    if (Queue_index.find(trans)!=Queue_index.end()){
        int Q_ID = Queue_index.find(trans)->second;
        return &Queue_repository[Q_ID];
    }
    else
    {
        return NULL;
    }



}


void Highways::presetQueue (tlm::tlm_generic_payload* trans, int numFlits) {
    int Q_ID = Queue_index.find(trans)->second;

    Queue_repository[Q_ID].currentPointer = numFlits;
   // cout <<_name << " trans "<<hex<<trans->get_address()<<" flit is preset to " <<dec<<Queue_repository[Q_ID].currentPointer <<endl;
}

void Highways::markRelease(tlm_generic_payload* trans) {
    int Q_ID = Queue_index.find(trans)->second;
    Queue_repository[Q_ID].release_marker = true;
    //Queue_repository[Q_ID].push();
}



void Highways::receiveAFlit(tlm_generic_payload* trans){
    int Q_ID = Queue_index.find(trans)->second;
    Queue_repository[Q_ID].push();
}

bool Highways::checkHit(tlm_generic_payload* trans){
    int Q_ID = Queue_index.find(trans)->second;
    if (Queue_repository[Q_ID].refill_slot == Slot.readPositive()) {
        return true;
    }
    return false;
}

bool Highways::sendAFlit (tlm_generic_payload* trans){                                            // not very efficient
    int Q_ID = Queue_index.find(trans)->second;
   // cout <<_name << " trans "<<hex<<trans->get_address()<<" remian flits are " <<dec<<Queue_repository[Q_ID].currentPointer <<endl;
    Queue_repository[Q_ID].pop();
    if (Queue_repository[Q_ID].release_marker && Queue_repository[Q_ID].isEmpty()) {
        return false;
    }
    return true;

}

bool Highways::isEmpty(tlm_generic_payload * trans) {
    int Q_ID = Queue_index.find(trans)->second;
    return Queue_repository[Q_ID].isEmpty();

}

bool Highways::isFull(tlm_generic_payload * trans) {
    int Q_ID = Queue_index.find(trans)->second;
    return Queue_repository[Q_ID].isFull();

}

int Highways::checkToken(tlm_generic_payload * trans) {
    int Q_ID = Queue_index.find(trans)->second;
    return Queue_repository[Q_ID].getToken();
}

bool Highways::readyToSend(tlm_generic_payload* trans) {
    int Q_ID = Queue_index.find(trans)->second;


    if (Queue_repository[Q_ID].nextQueueToken >0 && !isEmpty (trans))
    {
        return true;
    }
    else
        return false;
}

void Highways::updateNextToken(tlm_generic_payload* trans, int num) {
    int Q_ID = Queue_index.find(trans)->second;
    Queue_repository[Q_ID].nextQueueToken=num;

}

void Highways::IncreNextToken(tlm_generic_payload* trans){
    int Q_ID = Queue_index.find(trans)->second;
    if ( Queue_repository[Q_ID].nextQueueToken < HIGHWAYDEPTH)
        Queue_repository[Q_ID].nextQueueToken++;
    //cout << "token after increase is Queue_repository[Q_ID].nextQueueToken"
}

void Highways::DecNextToken(tlm_generic_payload* trans){
    int Q_ID = Queue_index.find(trans)->second;
    Queue_repository[Q_ID].nextQueueToken--;
}


vector <tlm::tlm_generic_payload*> Highways::weighted_arbitrate(vector <bool> slots) {
    //int currentRoundPointer;
    vector < tlm::tlm_generic_payload* > trans;
    trans.resize(_numDirections,NULL);

    // here, we can check with slot table to make sure;
    for (int i = 0; i <_numOfQueues; i ++){
        if (Queue_repository[i].refill_slot == SlotManager::readPositive()  && !Queue_repository[i].isEmpty()  ){ //?

            //_input_arb[i/NUM_DIRECTIONS]->rotateState(i%NUM_DIRECTIONS);
            const int output=Queue_repository[i].out_direction;
            //_output_arb[output]->rotateState(i/NUM_DIRECTIONS);
            trans[output]= Queue_repository[i].trans;
            Queue_repository[i].refill();
          //  cout <<_name << " queue " << i <<" output "<<output << hex<<  setfill('0') << setw(4)<< " trans " <<Queue_repository[i].trans->get_address() << " refill  slot is hit " << Queue_repository[i].refill_slot <<endl;

        }

    }

    for (int i = 0; i < _numDirections; ++i) {
        for (int j =_numEachInDirect*i; j <(i+1)*_numEachInDirect;j++){

            int output= Queue_repository[j].out_direction;
            if (output!=Nodrctn ) {
              //  cout << "lala "<<endl;
#ifndef CONCISE
            cout <<_name << " queue " << j <<" output "<<output << hex<<  setfill('0') << setw(4)<< " trans " <<Queue_repository[j].trans->get_address() << dec<<" refill  slot is " << Queue_repository[j].refill_slot <<  " slot is free" << slots[output];
            cout <<_name << " token " << Queue_repository[j].nextQueueToken<< " remain flit "<<Queue_repository[j].currentPointer<<endl;
#endif
            }
            // cout <<" next token of q " <<j <<" is " << Queue_repository[j].nextQueueToken<<endl;
            if (output!= Nodrctn &&   !Queue_repository[j].isEmpty() && Queue_repository[j].nextQueueToken >0
                    && slots[output] && !Queue_repository[j].tailFlit()) {



                if (Queue_repository[j].weight_counter>0) {

                    _input_arb[i]->AddRequest(j-_numEachInDirect*i, j,1);
                }
                else
                {
                    _input_arb[i]->AddRequest(j-_numEachInDirect*i, j,0);
                }
            }
        }

    }



    for (int i=0; i<_numDirections; i++){

        if (_input_arb[i]->_num_reqs>0){
            int id = -1;
            int pri= -1;
            _input_arb[i]->Arbitrate(&id, &pri);
            const int output = Queue_repository[id].out_direction;
            //cout << "output_arb" << output << "add input"<<i << "its position is "<<_input_arb[i]->current_pointer()<<endl;
            assert(output > -1);
            _output_arb[output]->AddRequest(i, id, pri);
        }
    }


    for (int i=0; i<_numDirections; i++)
    {
        if (_output_arb[i]->_num_reqs>0){
            int id = -1;
            int pri = -1;
            const int input = _output_arb[i]->Arbitrate(&id, &pri);
            assert(input > -1);
            //register connection table
            trans[i]= Queue_repository[id].trans;
            //            if (pri>0){
            //                Queue_repository[id].weight--;
            //                assert (Queue_repository[id].weight >= 0 );
            //            }

            _input_arb[input]->UpdateState() ;
            _output_arb[i]->UpdateState() ;
            //  cout << "output_arb" << i << "grant input"<<input << "its position is "<<_input_arb[input]->current_pointer()<<endl;
            _output_arb[i]->Clear();
        }
        _input_arb[i]->Clear();

    }

    //


    return trans;

}

bool Highways::allIsEmpty() {
    for (int i = 0; i <_numOfQueues; ++i) {
        if (!Queue_repository[i].isEmpty() || Queue_repository[i].release_marker) {
            return false;
        }
    }
    return true;
}




