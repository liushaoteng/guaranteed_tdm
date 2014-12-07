/*xt
 * TDMRouter.cpp
 *
 *  Created on: Mar 4, 2013
 *      Author: lsteng
 */
//#define DETAIL
#include "TDMRouter.h"


using namespace std;
using namespace PPB;
using namespace sc_core;
using namespace tlm;

void TDM_Router::getAim (ProbeProperty * property, Probe_extension *extension) {
    (this->*_routeFunc) (property, extension);

}

int TDM_Router::cancelMethod_simple(int inId_unused, tlm_generic_payload *trans, NewTrans_itr itr){

    int t = -1;
    int cancelVector=0;
    int inId = itr->second.inputChannel.id;


    if (queueOfAllInputs->isAvailable(inId) && itr->second.ownAQueue)
    {
        cancelVector=1;

    }

    itr++;
    inId = itr->second.inputChannel.id;
    if (queueOfAllInputs->isAvailable(inId) && itr->second.ownAQueue) {
        cancelVector = cancelVector +2;
    }

    if (cancelVector == 0 || cancelVector ==3) {
        t = 1;
    }
    else if (cancelVector == 1 )
    {
        t = 1;
    } else if ( cancelVector == 2) {
        t = 0;
    }

    //  std::cout << sc_time_stamp() << " " << name() << " trans " << hex << setfill('0') << setw(4) << trans->get_address() <<  "cancelVector " << cancelVector << " t " << t<<" inId" << inId<< endl ;
    assert (t>=0);
    return t;

}

void TDM_Router::adaptive(ProbeProperty * property, Probe_extension *extension) {


    //destination is reached
    if ((extension->dstCol == _node_col) && (extension->dstRow == _node_row)) {
        property->aimingDirections[0] = Resource;
        property->aimingDirections[1] = Nodrctn;
        return;
    }
    //get col aimed direction
    if (extension->dstCol == _node_col) {
        property->aimingDirections[0] = Nodrctn;
    } else if (extension->dstCol > _node_col) {
        property->aimingDirections[0] = North;
    } else
        property->aimingDirections[0] = South;
    //get row aimed direction
    if (extension->dstRow == _node_row) {
        property->aimingDirections[1] = Nodrctn;
    } else if ((extension->dstRow > _node_row)) {
        property->aimingDirections[1] = East;
    } else
        property->aimingDirections[1] = West;

    return;
}

void TDM_Router::XY(ProbeProperty * property, Probe_extension *extension){
    property->aimingDirections[0] = Nodrctn;
    property->aimingDirections[1] = Nodrctn;

    if ((extension->dstCol == _node_col) && (extension->dstRow == _node_row)) {
        property->aimingDirections[0] = Resource;
        property->aimingDirections[1] = Nodrctn;
        return;
    }
    if (extension->dstCol == _node_col) {
        property->aimingDirections[0] = Nodrctn;
        if (extension->dstRow == _node_row) {
            property->aimingDirections[1] = Nodrctn;
        } else if ((extension->dstRow > _node_row)) {
            property->aimingDirections[1] = East;
        } else
            property->aimingDirections[1] = West;

    } else if (extension->dstCol > _node_col) {
        property->aimingDirections[0] = North;
    } else
        property->aimingDirections[0] = South;
    return;


}

tlm::tlm_sync_enum TDM_Router::in_nb_transport_fw(int id,
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
        sc_core::sc_time& delay) {
    map<tlm_generic_payload*, ProbeProperty>::iterator itr;
    Probe_extension *extension;

    if (phase == tlm::BEGIN_REQ) {
        ProbeProperty property;

        unsigned char* data;

        /** analyze the incoming transactions*/
        trans.get_extension(extension);
        data = trans.get_data_ptr();
        property.srcId = extension->srcId;
        property.dstId = extension->dstId;

        property.highPriority = 0;
        property.lowPriority = id;
        //property->inputChannel->id = id;
        property.multiChannelProbe = extension->multiChannelProbe;

        property.trans = &trans;
        property.inputChannel.id=id;
        property.ownAQueue = extension->haveQueue;

        //               std::cout << sc_time_stamp() << " " << name() << " in from " << id
        //                        << " " << phase << " transaction: " << property.srcId << " "
        //                        << property.dstId << endl;


        itr = bookedTrans.find(&trans); // the storage must be used very carefully

        //                if (itr != tempProbes.end())
        //                    cout << name() << itr->first << " transaction "
        //                            << itr->second.dstId << itr->second.srcId << " is wrong "
        //                            << " current trans " << hex << setfill('0') << setw(4)
        //                            << trans.get_address() << endl;

        assert(itr==bookedTrans.end());
        //the input must be free when new probe come in

        if (itr == bookedTrans.end()) {
            //            storage.registerInChannel(id, property);
            //            storage.inputChannels_map[id] = property; // the storage must  used very carefully, remember to release it


            getAim(&property, extension);
#ifndef CONCISE
            std::cout << sc_time_stamp() << " " << name() << " in from input " << id
                    << " " << phase << " trans " << hex << setfill('0') << setw(4)
                    << trans.get_address() <<" aiming "
                    << get_direction_vec()[property.aimingDirections[0]]
                                           << " and "
                                           << get_direction_vec()[property.aimingDirections[1]]
                                                                  << " dest " << extension->dstRow << extension->dstCol
                                                                  << " me "<<_node_row<<_node_col<< " have a queue" << property.ownAQueue << endl;
#endif

#ifdef debug
            cout << "aiming direction is "
                    << get_direction_vec()[property->aimingDirections[0]]
                                           << " and "
                                           << get_direction_vec()[property->aimingDirections[1]]
                                                                  << endl;
#endif

            // return tlm::TLM_ACCEPTED;
            tempProbes.insert(
                    std::pair<tlm::tlm_generic_payload*, ProbeProperty>(&trans,
                            property));
            //MULTIPAYLOAD temp = MULTIPAYLOAD(id, PAYLOAD(&trans, phase));
            sc_time delay = sc_time(SLOTWIDTH, SC_NS);
            fw_map.notify(trans, phase, delay);

            //fw_stage1_event.notify(0, SC_NS);
        }

    } else if (phase == dataTransfer) { /** for accleration purpose*/

        itr = bookedTrans.find(&trans);
        trans.get_extension(extension);
        itr->second.tryCheck =extension->tryCheckQueue;
        // (*out_socket[outId])->nb_transport_fw(trans, phase, delay);
        sc_time delay = sc_time(SLOTWIDTH, SC_NS);
        fw_map.notify(trans, phase, delay);

    } else if (phase == cancelDownwardLinks) {

        //#ifndef CONCISE
        //        std::cout << sc_time_stamp() << " " << name() << " in from input "
        //                << id << " " << phase << " trans " << hex << setfill('0')
        //                << setw(4) << trans.get_address() << endl;
        //#endif
        sc_time delay = sc_time(SLOTWIDTH, SC_NS);
        fw_map.notify(trans, phase, delay);

        //fw_stage1_event.notify(1, SC_NS);
    }

    return tlm::TLM_ACCEPTED;
}


void TDM_Router::_fw_stage_1(tlm::tlm_generic_payload& trans,
        const tlm::tlm_phase& phase) {

    //same trans cancelation
    std::map<tlm::tlm_generic_payload*, ProbeProperty>::iterator itr;
    tlm::tlm_generic_payload * multiTrans;
    tlm::tlm_phase multiPhase;
    sc_core::sc_time delay;
    itr = tempProbes.find(&trans);
    Probe_extension *extension;
    trans.get_extension(extension);
    //add by shaoteng

    int currentPosSLot=Slot.readPositive();

    //   while ((phaseItr = fw_map.get_next_transaction())) {

    //trans = phaseItr->second.first;
    // phase = phaseItr->second.second;
#ifndef CONCISE
    std::cout << sc_time_stamp() << " " << name() <<" after 1 reg"
            << " " << phase << " trans " << hex << setfill('0') << setw(4)
            << trans.get_address() << " slot " << Slot.readPositive()<< " try check" <<extension->tryCheckQueue << " has queue " <<extension->haveQueue << " confirmed queue " <<extension->confirmWithQueue << endl;
#endif

    /*follwing code for same probe cancellation*/
    if (phase == tlm::BEGIN_REQ) {
        // #ifndef CONCISE
        //  std::cout << sc_time_stamp() << " " << name() << " " << phase << " trans " << hex << setfill('0')
        //                 << setw(4) << trans.get_address() << " deltain "<< sc_delta_count()<< endl;
        // #endif
        int inId = itr->second.inputChannel.id;
#ifndef CONCISE
        //  if (itr == tempTrans.end())
        //
        //  {
        //      for (itr = tempTrans.begin();itr!=tempTrans.end();itr++)
        //      {
        //          int inId = itr->second->inputChannel->id;
        //          std::cout << sc_time_stamp() << " " << name() << " in from input "
        //                << inId << " " << phase << " trans " << hex << setfill('0')
        //                << setw(4) << itr->first->get_address() << endl;
        //      }
        //  }
#endif
        assert(itr!=tempProbes.end());

        // occupied_output[inId]=true;

        if (tempProbes.count(itr->first) > 1) {
            itr->first->get_extension(extension);
            int t = cancelMethod_simple(inId, itr->first,itr);



            if (t)
                itr++;

            multiTrans = (itr->first);
            multiPhase = cancelOrigin;
            int bwId = itr->second.inputChannel.id;
            //occupied_input[bwId]=false;
            delay= sc_time(Slot.PosToNegSlot(),SC_NS);
            if (delay.value() == 0)
                delay = sc_time(SLOTDEPTH*SLOTWIDTH,SC_NS);
            bw_content[currentPosSLot][bwId]=multiPhase;

            MULTIPAYLOAD temp = MULTIPAYLOAD(bwId, PAYLOAD(multiTrans, multiPhase));
            bw_map.insert(pair <sc_time, MULTIPAYLOAD>(sc_time_stamp()+delay,temp));
            bw_event.notify (delay);
            //(*in_socket[bwId])->nb_transport_bw(*multiTrans, multiPhase, delay);




        } else {

            if (itr->second.ownAQueue ) {              //can be optimized for performance
                itr->second.ownAQueue= queueOfAllInputs->isAvailable(inId);
            }


            newTrans.insert(
                    std::pair<tlm::tlm_generic_payload*, ProbeProperty>(
                            itr->first, itr->second));
            // itr++;
#ifdef DETAIL
            std::cout << sc_time_stamp() << " " << name()
                                                                                                                                    << " insert into unfixed queue " << " trans " << hex
                                                                                                                                    << setfill('0') << setw(4) << itr->first->get_address()
                                                                                                                                    << endl;
#endif
            assert(
                    ((itr->second).aimingDirections[0] != Nodrctn||(itr->second).aimingDirections[1] != Nodrctn));


        }
        tempProbes.erase(itr);

        delay = sc_time(0, SC_NS);

        fw_stage2_event.notify(delay);

    } else if (phase == dataTransfer) {
        //Probe_extension * extension;
        //trans.get_extension(extension);
        NewTrans_itr nItr = bookedTrans.find(&trans);
        int inId = nItr->second.inputChannel.id;
        int outId = nItr->second.outputChannel[0].id;
        //int regInId =slotTable.readSlots(Slot.readPositive())[outId];

        if (nItr->second.tryCheck ) {
            if (queueOfAllInputs->getTheQueue(&trans)) {
                queueOfAllInputs->release(&trans);
            }
            if (extension->haveQueue ) {              //can be optimized for performance
                extension->haveQueue= queueOfAllInputs->isAvailable(inId);
            }


        }
        extension->tryCheckQueue = nItr->second.tryCheck;
        // assert (!queueOfAllInputs->isFull(&trans));
        if (!extension->confirmWithQueue || (extension->confirmWithQueue && queueOfAllInputs->isEmpty(&trans) && queueOfAllInputs->checkHit(&trans) ) ){


            assert( inId==slotTable.readSlots(Slot.readPositive())[outId] );



            delay= sc_time(0,SC_NS);
            //            int inId = nItr->second.inputChannel.id;
            //            int outId = nItr->second.outputChannel[0].id;
            //            assert( inId==slotTable.readSlots(Slot.readPositive())[outId] );
            tlm::tlm_phase fwphase = phase;
            (*out_socket[outId])->nb_transport_fw(trans, fwphase, delay);
        } else {
            // if (!queueOfAllInputs->checkHit(&trans))
            assert (!queueOfAllInputs->isFull(&trans) || queueOfAllInputs->checkHit(&trans));


            if (queueOfAllInputs->isEmpty(&trans)) {
                fw_stage3_event.notify();        // notify at the begining of a cycle
            } else {

                delay = sc_time(1, SC_NS);

                fw_stage3_event.notify(delay);
            }
            queueOfAllInputs->receiveAFlit(&trans);
            //            int bwId=queueOfAllInputs->getTheQueue(&trans)->in_direction;
            //            tlm::tlm_phase multiPhase = feedbackCredit;
            //            MULTIPAYLOAD temp = MULTIPAYLOAD(bwId, PAYLOAD(&trans, multiPhase));
            //            delay = (SLOTWIDTH,SC_NS);
            //            bw_map.insert(pair <sc_time, MULTIPAYLOAD>(sc_time_stamp(),temp));
            //            bw_event.notify ();
            //            erase_event.notify(delay);

        }



    } else if (phase == cancelDownwardLinks) {
        trans.get_extension(extension);
        NewTrans_itr nItr = bookedTrans.find(&trans);
        int outId = nItr->second.outputChannel[0].id;
        //int inId = nItr->second.inputChannel.id;
        int regInId =slotTable.readSlots(Slot.readPositive())[outId];
        if (!extension->confirmWithQueue ){   // refill slot is hit and queue is empty, there are some risks



            assert (nItr != bookedTrans.end());
            delay= sc_time(0,SC_NS);
            int inId = nItr->second.inputChannel.id;



            assert( inId==slotTable.readSlots(Slot.readPositive())[outId] );


            slotTable.markErasedSlot(currentPosSLot,outId);

            bookedTrans.erase(nItr);

            tlm::tlm_phase fwphase;
            fwphase = cancelDownwardLinks;
            (*out_socket[outId])->nb_transport_fw(trans, fwphase, delay);
            delay = sc_time (1,SC_NS);
            erase_event.notify(delay);
            //            if (queueOfAllInputs->getTheQueue(&trans))
            //            { queueOfAllInputs->markRelease(&trans);
            //            fw_stage3_event.notify(1,SC_NS);
            //            }

        } else if (extension->confirmWithQueue && queueOfAllInputs->isEmpty(&trans) && queueOfAllInputs->checkHit(&trans) ) {
            assert (nItr != bookedTrans.end());
            delay= sc_time(0,SC_NS);
            int inId = nItr->second.inputChannel.id;



            assert( inId==slotTable.readSlots(Slot.readPositive())[outId] );



            slotTable.markErasedSlot(currentPosSLot,outId);

            bookedTrans.erase(nItr);

            tlm::tlm_phase fwphase;
            fwphase = cancelDownwardLinks;
            (*out_socket[outId])->nb_transport_fw(trans, fwphase, delay);
            delay = sc_time (1,SC_NS);
            erase_event.notify(delay);
        }

        if (extension->confirmWithQueue ) {
            queueOfAllInputs->markRelease(&trans);
            if (!queueOfAllInputs->isEmpty(&trans))
                queueOfAllInputs->receiveAFlit(&trans);

            fw_stage3_event.notify(1,SC_NS);
        } else  {
            if (queueOfAllInputs->getTheQueue(&trans)) {


                queueOfAllInputs->release(&trans);
                //cout << " empty" << queu}eOfAllInputs->isEmpty(&trans) << " in id " << inId << " regId " << regInId <<endl;
            }
        }
    }
    // fw_map.erase(phaseItr++);
    //   }
}

void TDM_Router::_fw_stage_2() {

    //std::map<ProbeProperty*, tlm::tlm_generic_payload*>::iterator unfixedItr; //unfixed channel itr;
    std::map<tlm::tlm_generic_payload*, ProbeProperty>::iterator tempItr;
    std::map<tlm::tlm_generic_payload*, ProbeProperty>::iterator bookedItr;
    std::vector<Channel>::iterator chItr;
    tlm::tlm_generic_payload * trans;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
#ifdef DETAIL_INFORM
    stage_2_counter++;
    cout << name() << " stage 2 counter is " << stage_2_counter << endl;
#endif
#ifndef CONCISE
    std::cout << sc_time_stamp() << " " << name() << " begin allocation " <<endl;
#endif
    //storage.round_robin_16to4_allocation();
    //storage.input_fist_allocation();
    int currentPosSLot = Slot.readPositive();
    allocator->Allocate(currentPosSLot);
    // storage.wavefront_allocation();


#ifndef CONCISE
    std::cout << sc_time_stamp() << " " << name() << " end allocation " <<endl;
#endif
    //allocate channels and issue channel cancellation;
    for (tempItr=newTrans.begin(); tempItr!=newTrans.end(); tempItr++) {
        trans = tempItr->first;
        Probe_extension *extension;
        trans->get_extension(extension);
        int bwId = tempItr->second.inputChannel.id;
        //    cout << name()<<"begin iteration" <<endl;
        if (tempItr->second.outputChannel.empty()) {
            phase = cancelOrigin;

            delay= sc_time(Slot.PosToNegSlot(),SC_NS);
            if (delay.value() == 0)
                delay = sc_time(SLOTDEPTH*SLOTWIDTH,SC_NS);
            bw_content[currentPosSLot][bwId]=phase;
            MULTIPAYLOAD temp = MULTIPAYLOAD(bwId, PAYLOAD(trans, phase));
            //bw_map.notify(temp,delay);
            bw_map.insert(pair <sc_time, MULTIPAYLOAD>(sc_time_stamp()+delay,temp));
            bw_event.notify (delay);
#ifndef CONCISE
            std::cout << sc_time_stamp() << " " << name() << " begin to cancel "
                    <<bwId << " " << phase << " trans " << hex << setfill('0')
                    << setw(4) << trans->get_address() << "cancel time is "  << delay << endl;
#endif


            if (queueOfAllInputs->getTheQueue(trans)) {
                queueOfAllInputs->release(trans);

            }
#ifdef DETAIL
            std::cout << sc_time_stamp() << " " << name() << " release "
                    << " " << phase << " trans " << hex << setfill('0')
                    << setw(4) << trans->get_address()
                    << endl;
#endif

        } else
        {
            phase = tlm::BEGIN_REQ;
            delay = sc_time(SLOTDEPTH*SLOTWIDTH, SC_NS);


            for (chItr=tempItr->second.outputChannel.begin();chItr!=tempItr->second.outputChannel.end(); chItr++)
            {
                int outId = chItr->id;
                int size =tempItr->second.outputChannel.size();
                extension->haveQueue= tempItr->second.ownAQueue;
#ifdef DETAIL
                std::cout << sc_time_stamp() << " " << name() << " write slot " <<currentPosSLot << " in " << bwId << " out " <<outId <<" queue size " <<
                        size << endl;
#endif
                (*out_socket[outId])->nb_transport_fw(*trans, phase, delay);
#ifndef CONCISE
                std::cout << sc_time_stamp() << " " << name() << " trans " << trans->get_address() << " output" << outId << " is occupied " <<endl;
#endif

                occupied_output[outId]=true;
                slotTable.writeASlot(currentPosSLot,bwId,outId);
                slotTable.updateSlotStatus(currentPosSLot,outId,Booked);
                bookedTrans.insert(*tempItr);
                //chItr++;
            }

            //
            //            }
        }
    }
    newTrans.clear();
    //cout <<"la"<<endl;

    transForSendInCurCycle= queueOfAllInputs->weighted_arbitrate(slotTable.checkConfirms(Slot.readPositive()));

    //    for (int i =0; i<NUM_DIRECTIONS; i++) {
    //        if (transForSendInNextCycle[i]){
    //            fw_stage3_event.notify(1,SC_NS);

    //
    //            break;
    //        }
    //
    //    }
#ifndef CONCISE
    for ( int i = 0; i < NUM_DIRECTIONS; i ++) {

        if (occupied_output[i]) {
            std::cout << sc_time_stamp() << " " << name() << " output" << i << " is occupied " <<endl;
        }
    }
#endif
    //cout <<"lalala"<<endl;
    phase=tlm::BEGIN_REQ;
    tlm_generic_payload* trans_data;
    for (int i = 0; i < NUM_DIRECTIONS; ++i) {
        if (transForSendInCurCycle[i]!=NULL && !occupied_output[i])
        {
            trans_data = transForSendInCurCycle[i];
            int bwId=queueOfAllInputs->getTheQueue(trans_data)->in_direction;
            int hitSlot = queueOfAllInputs->getTheQueue(trans_data)->refill_slot;
            if (queueOfAllInputs->sendAFlit(trans_data)) {
                phase=dataTransfer;
            }
            else {
                phase=cancelDownwardLinks;
                slotTable.markErasedSlot(currentPosSLot,i);
                NewTrans_itr nItr = bookedTrans.find(trans_data);
                bookedTrans.erase(nItr);



                delay = sc_time (1,SC_NS);
                erase_event.notify(delay);
            }
#ifndef CONCISE
std::cout << sc_time_stamp() << " " << name() << " send data flit, input "
        <<bwId << " " << phase << " trans " << hex << setfill('0')
        << setw(4) << trans_data->get_address() << endl;
#endif

(*out_socket[i])->nb_transport_fw(*trans_data, phase, delay);

if (currentPosSLot != hitSlot) {
    tlm::tlm_phase multiPhase = feedbackCredit;
    MULTIPAYLOAD temp = MULTIPAYLOAD(bwId, PAYLOAD(trans_data, multiPhase));
    delay = sc_time(SLOTWIDTH,SC_NS);
    bw_map.insert(pair <sc_time, MULTIPAYLOAD>(sc_time_stamp(),temp));
    bw_event.notify ();
}
//erase_event.notify(delay);
        }
        occupied_output[i]=false;
        transForSendInCurCycle[i] = NULL;
    }



    queueOfAllInputs->release();
    if (!queueOfAllInputs->allIsEmpty() ) {
        fw_stage3_event.notify(SLOTWIDTH,SC_NS);
    }

    //fw_stage3_event.notify(1, SC_NS);
}


void TDM_Router::_erase_stage(){
    slotTable.eraseSlots();
#ifndef CONCISE
    std::cout << sc_time_stamp() << " " << name() << " erase stage " << endl;
#endif
    for (int i=0; i<SLOTDEPTH; i++)
        for (int j=0; j<NUM_DIRECTIONS; j++)
        {
            if (bw_content_cleanmark[i][j]== true)
            {
                bw_content_cleanmark[i][j]=false;
                bw_content[i][j]=tlm::BEGIN_REQ;
            }
        }




}


void TDM_Router::_fw_stage_3() {
    // transForSendInCurCycle = transForSendInNextCycle;
    // transForSendInNextCycle.resize(NUM_DIRECTIONS,NULL);


    //if (!queueOfAllInputs->allIsEmpty() ) {
    fw_stage2_event.notify(0,SC_NS);
    //}
    //    else {
    //        for (int i = 0; i < NUM_DIRECTIONS; ++i) {
    //
    //
    //            if (transForSendInCurCycle[i]){
    //                fw_stage2_event.notify(SLOTWIDTH,SC_NS);
    //
    //                break;
    //            }
    //        }
    //
    //    }


}

tlm::tlm_sync_enum TDM_Router::out_nb_transport_bw(int id,
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
        sc_core::sc_time& delay) {
    std::map<int, ProbeProperty*>::iterator inItr;
    // int inputId;
    //int currentNegSlot = Slot.readnegtive();
    // bw_map.erase(id);
    //  try {
    tlm::tlm_phase _phase=phase;
    int bwId=id;
    tlm::tlm_generic_payload * bwtrans = &trans;
    MULTIPAYLOAD temp = MULTIPAYLOAD(bwId, PAYLOAD(bwtrans, _phase));
    sc_time tempDelay(SLOTWIDTH, SC_NS);
    bw_map.insert(pair <sc_time, MULTIPAYLOAD>(sc_time_stamp()+tempDelay,temp));
    // bw_map.notify(temp,tempDelay);

    bw_event.notify(tempDelay);
    //#ifndef CONCISE
    //    std::cout << sc_time_stamp() << " " << name() << " in from output "
    //            <<id << " " << phase << " trans " << hex << setfill('0')
    //            << setw(4) << trans.get_address() << "current slot is " << Slot.readPositive() << endl;
    //#endif
    //    if (phase == cancelDueSameProbe || phase == cancelDueContention)
    //        {
    //
    //        }
    return tlm::TLM_ACCEPTED;
}

void TDM_Router::_bw_operation() {
    /**check input channels, if it has no outputs, cancel it*/
    // std::map<int, ProbeProperty*>::iterator inItr;
    // std::map<int, ProbeProperty*>::iterator outItr;
    std::vector<Channel>::iterator chItr;
    std::map<tlm::tlm_generic_payload*, ProbeProperty>::iterator transItr;
    std::multimap<const sc_core::sc_time, MULTIPAYLOAD>::iterator queueItr;
    MULTIPAYLOAD phaseItr;

    tlm::tlm_generic_payload * trans;
    tlm::tlm_phase phase;
    tlm::tlm_phase fwphase;
    sc_core::sc_time delay;
    delay = sc_core::sc_time(0, sc_core::SC_NS);

    int currentNegSlot = Slot.readnegtive();
    //    cout << sc_time_stamp() << " " << name() << "begin its bw_operation"
    //            << endl;
    //    cout << name() << "output channel_map size is "
    //            << storage.outputChannels_map.size() << endl;
    // assert(!bw_map.empty());
    while (bw_map.size()) {
        if (bw_map.begin()->first > sc_time_stamp()) {
            sc_time now= sc_time_stamp();
            sc_time when= bw_map.begin()->first;
            bw_event.notify (when-now);
            break;
        }
        queueItr = bw_map.begin();
        phaseItr=queueItr->second;
        trans = phaseItr.second.first;
        phase = phaseItr.second.second;


        int outId;
        int inId;
        if (phase != cancelOrigin)
        {
            outId= phaseItr.first;
#ifndef CONCISE
std::cout << sc_time_stamp() << " " << name() << " in from output " <<outId
        << " trans " << hex << setfill('0')
        << setw(4) << trans->get_address()<<" phase is "  << phase <<" current slot is " << currentNegSlot << endl;
#endif

transItr= bookedTrans.find(trans);


assert(transItr!=bookedTrans.end()||phase==haveCredit||phase==bookAqueue);


inId = transItr->second.inputChannel.id;
assert( slotTable.compare (currentNegSlot,inId, outId) || phase ==feedbackCredit || phase == haveCredit || phase==bookAqueue);


        }
        else
        {
            inId = phaseItr.first;
#ifndef CONCISE
            std::cout << sc_time_stamp() << " " << name() << " in from output "
                    << outId << " " << phase << " trans " << hex << setfill('0')
                    << setw(4) << trans->get_address() << " correspond inId "
                    << inId << "current slot is " << currentNegSlot << endl;
#endif
            assert (bw_content[currentNegSlot][inId]==cancelOrigin);
        }



        /** a check could be added here*/




        // inItr = storage.inputChannels_map.find(inId);
#ifdef DETAIL
        //if (inItr==storage.inputChannels_map.end())
        std::cout << sc_time_stamp() << " " << name() << " in from output "
                << outId << " " << phase << " trans " << hex << setfill('0')
                << setw(4) << trans->get_address() << " correspond inId "
                << inId << endl;
#endif

        // ProbeProperty* property = outItr->second;
        //#ifndef CONCISE
        //        std::cout << sc_time_stamp() << " " << name() << " in from output "
        //                << outId << " " << phase << " trans " << hex << setfill('0')
        //                << setw(4) << trans->get_address() << " correspond inId "
        //                << inId << "current slot is " << currentNegSlot << endl;
        //#endif
        if (phase == cancelDueSameProbe || phase == cancelDueContention) {



            fwphase = phase;

            // assert (slotTable.compare (currentNegSlot,inId, outId));

            if (!transItr->second.outputChannel.empty()) {
                for (chItr = transItr->second.outputChannel.begin();
                        chItr != transItr->second.outputChannel.end();) {
                    //std::vector<Channel*>::iterator tmpchItr = inItr;

                    if (chItr->id == outId) {
                        chItr = transItr->second.outputChannel.erase(chItr);
                        slotTable.markErasedSlot(currentNegSlot,outId);
                        //
                        erase_event.notify(1,SC_NS);


                    } else
                        chItr++;
                }
                //inItr++;
            }

            if (transItr->second.outputChannel.empty()) {

#ifdef DETAIL
                std::cout << sc_time_stamp() << " " << name() << " release "
                        << " " << phase << " trans " << hex << setfill('0')
                        << setw(4) << trans->get_address() << endl;
#endif

                /** or, a check could be add here */
                (*in_socket[inId])->nb_transport_bw(*trans, fwphase, delay);
                if (queueOfAllInputs->getTheQueue(trans)) {
                    queueOfAllInputs->release(trans);
                }
                bookedTrans.erase(trans);
                // slotTable.markErasedSlot(currentNegSlot,outId);
                // erase_event.notify (1,SC_NS);

                //storage.unfixedChannels.erase(inItr->second);
            }
        } else  if (phase == cancelOrigin)
        {
            if (bw_content_cleanmark[currentNegSlot][inId] != false)
                throw Router_err ("the bw content has already been marked",_name);
            bw_content_cleanmark[currentNegSlot][inId];
            erase_event.notify (1,SC_NS);



            fwphase = cancelDueContention;   // HERE CAN BE IMPROVED TO differentiate the cause of fail
#ifdef DETAIL
            std::cout << sc_time_stamp() << " " << name() << " release "
                    << " " << phase << " trans " << hex << setfill('0')
                    << setw(4) << trans->get_address() << endl;
#endif
            (*in_socket[inId])->nb_transport_bw(*trans, fwphase, delay);

        }

        else if (phase == succeed) {
            Probe_extension *extension;
            trans->get_extension(extension);
            if (extension->confirmWithQueue ==0) {
                if (queueOfAllInputs->getTheQueue(trans)) {
                    queueOfAllInputs->release(trans);
                    cout <<"false in queue allocation" <<endl;
                    assert (false);
                }
            }else
            {

                if ( queueOfAllInputs->allocate(trans,inId)) {
                    queueOfAllInputs->configure(trans,outId);
                }
                else {
                    extension->confirmWithQueue =false;
                }

            }
            slotTable.confirmASlot(currentNegSlot,outId);
            //  assert( slotTable.compare (currentNegSlot,inId, outId));
            (*in_socket[inId])->nb_transport_bw(*trans, phase, delay);
        }else if (phase == feedbackCredit){
            //int feedbackToken= queueOfAllInputs->checkToken(trans );
            // if (feedbackToken >=3)
            // {
            fwphase = haveCredit;
            //  }
            //  else {
            //      fwphase = noCredit;
            // }
            (*in_socket[inId])->nb_transport_bw(*trans, fwphase, delay);

        }else if (phase == haveCredit) {
            if (queueOfAllInputs->getTheQueue(trans)) {

                if (!queueOfAllInputs->getTheQueue(trans)->valid)
                    queueOfAllInputs->IncreNextToken(trans);
            }


        } else if (phase == bookAqueue) {
            Probe_extension *extension;
            trans->get_extension(extension);
            if (extension->confirmQueueInTransfer ==0) {
                //                if (queueOfAllInputs->getTheQueue(trans)) {
                //                    queueOfAllInputs->release(trans);
                //                }
                //assert (false);
            }else
            {
#ifndef CONCISE
                std::cout << sc_time_stamp() << " " << name() << " in from output " <<outId
                        << " trans " << hex << setfill('0')
                        << setw(4) << trans->get_address()<<" phase is "  << phase <<" current slot is " << currentNegSlot << " source slot " <<extension->src_slotId << " has queue " << extension->confirmQueueInTransfer<< endl;
#endif

                if ( queueOfAllInputs->allocate(trans,inId)) {
                    queueOfAllInputs->configure(trans,outId);
                }
                else {
                    extension->confirmQueueInTransfer =0;
                }

            }
            (*in_socket[inId])->nb_transport_bw(*trans, phase, delay);

        }
        //        else if (phase == noCredit) {
        //            queueOfAllInputs->updateNextToken(trans,0);
        //        }

        else {
            std::cout << "_ERROR:" << "no such phase" << std::endl;
            assert(false);
        }
        // bw_map.erase(phaseItr++);
        bw_map.erase(queueItr);
    }
}
