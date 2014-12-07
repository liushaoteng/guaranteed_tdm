/**
 *  in this release, I modified the following:
 *  1. The resource node do not have to wait for previous transaction to be finished to send the new one
 *     It can send out as many as transactions if free channel exists.
 *  2. When it retries, it uses the same channel in which it has been first send out;
 *
 *  3. the retry waiting time should be reduced
 */

#include "simpleresource_TDM.h"
#include "networkdef.h"
#include <fstream>
#include <iomanip>
#include <stdlib.h>

using namespace sc_core;
using namespace std;
using namespace sc_dt;
using namespace PPB;



void SimpleResource_TDM::standardSend_process_until_success() {
    tlm::tlm_generic_payload* trans;
    tlm::tlm_phase phase;
    sc_core::sc_time delay;
    request_monitor* req_itr;
    // Generate a sequence of random transactions
    while (!infile.eof()) {
        //        sc_uint<4> _dest_col = (rand() % COL); // dest address col
        //        sc_uint<4> _dest_row = (rand() % ROW); // dest address row
        wait(next_request_event);
        //    cout << sc_time_stamp() << " " << name() << " next_request_event is triggered " <<
        //             "progress lock is " <<endl;

        while (!request_queue.empty()) {
            /**remeber, col should always comes first, row second*/
            int req_id = *request_queue.begin();
            req_itr = pending_req.find(req_id)->second;
            //
            //            int currentTime = int(sc_time_stamp().value());
            //            int consumedTime = currentTime - req_itr->gen_time;
            //            int singleTry_time = req_itr->distance * 3 + 4+SLOTDEPTH*SLOTWIDTH;
            //            int residue_time = req_itr->LT_req - consumedTime;

            //                    if (residue_time <singleTry_time)
            //                    {
            //                        delete req_itr;
            //                        pending_req[req_id] = 0;
            //                        pending_req.erase(req_id);
            //                        request_queue.pop_front();
            //
            int currentTime, consumedTime, singleTry_time, residue_time;
            //                    }
            //                    else
            //                    {
            //                        cout << "error, can not happen" << endl;
            //                        assert (false);
            //                    }
            //         cout<<name()<<" request throughput is " <<throughput <<endl;
            //double throughput = req_itr->THG_req;
            //for (int i = 0; i < ceil(throughput); i++) {
            // if (residue_time - singleTry_time > 0) {
            for (int i = 0; i < ceil(SLOTDEPTH); i++) {
                progress_lock.wait();
                int req_id = *request_queue.begin();
                req_itr = pending_req.find(req_id)->second;
                if (pending_req.find(req_id) == pending_req.end()) {
                    progress_lock.post();
                    break;

                }
                if (req_itr->status != Generated
                        && req_itr->status != Sendout) {
                    //case 1: PatialSucceed is okay;
                    // you must be very very be careful here, since it is possible
                    //that request has been released before it all send out

                    //      cout<<name()<<" request  has finished " <<throughput <<endl;
                    progress_lock.post();

                    break;
                }


                currentTime = int(sc_time_stamp().value());
                consumedTime = currentTime - req_itr->gen_time;
                singleTry_time = req_itr->distance * 3 + 4 + SLOTDEPTH * SLOTWIDTH;
                residue_time = req_itr->LT_req - consumedTime;
                //            if (residue_time < singleTry_time) {
                //                //      cout<<name()<<" request  has finished " <<throughput <<endl;
                //                progress_lock.post();
                //                if (i == 0) {
                //                    delete req_itr;
                //                    pending_req[req_id] = 0;
                //                    pending_req.erase(req_id);
                //                }
                //                break;
                //            }

                sc_uint<4> _dest_col = req_itr->col; // dest address col
                sc_uint<4> _dest_row = req_itr->row; // dest address row

                sc_uint<4> _src_col = _node_col;
                sc_uint<4> _src_row = _node_row;

                sc_uint<8> _src_id = (_src_row, _src_col);
                sc_uint<8> _dst_id = (_dest_row, _dest_col);

                sc_uint<16> _addr = (_dest_row, _dest_col, _src_row, _src_col);
                unsigned int addr = _addr.to_int();

                // int port_num = rand() % NUM_CHANNELS;
                Probe_extension *extension = new Probe_extension;
                // Grab a new transaction from the memory manager
                trans = m_mm.allocate();
                assert (trans);
                trans->acquire();

                // Set all attributes except byte_enable_length and extensions (unused)
                //trans->set_command( cmd );
                trans->set_address(addr);
                trans->set_data_ptr(reinterpret_cast<unsigned char*>(priority));
                //trans->set_data_length( 4 );
                //trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
                trans->set_byte_enable_ptr(
                        reinterpret_cast<unsigned char*>(&request)); // 0 indicates unused
                trans->set_dmi_allowed(false); // Mandatory initial value
                trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); // Mandatory initial value
                trans->set_extension(extension); //this is not autoextension, so please remember delete it when release

                extension->dstCol = _dest_col;
                extension->dstRow = _dest_row;
                extension->srcCol = _src_col;
                extension->srcRow = _src_row;
                extension->srcId = _src_id.to_int();
                extension->dstId = _dst_id.to_int();
                extension->r_id = req_itr->r_id;

                extension->multiChannelProbe = 0;
                extension->probeType = SingleProbe;



                // int genTime = req_itr->gen_time;
                //  gen_time[trans] = genTime;
                // Initiator must honor BEGIN_REQ/END_REQ exclusion rule
                //            if (progress_lock !=0 || request_in_progress!=0) {
                //                wait(end_request_event);
                //
                //            }

                int id = 0;
                do {
                    //id = int(rand() % RESOURCE_CHANNEL_NUM); //randomize the send_out port;
                    id++;
                } while (request_in_progress[id % SLOTDEPTH] != NULL);
                id = id % SLOTDEPTH;
                //#ifndef CONCISE
                //            cout << dec << int(sc_time_stamp().value()) << " ns " << name()
                //                                << "find " <<dec<<id<<" for "<< hex << setfill('0') << setw(4)
                //                                << trans->get_address() << endl;
                //#endif
                req_itr->add();
                request_in_progress[id] = trans;
                assert (request_in_progress[id] != 0);
                retry[id] = 0;
                phase = tlm::BEGIN_REQ;
                extension->src_slotId = id;



                // m_trans_req[trans] = req_itr->r_id;

                int wait = SlotManager::pos_waitingTime(id);

                if (wait == 0)
                    wait = SLOTDEPTH * SLOTWIDTH;
                //  cout << wait <<endl;

                delay = sc_time(wait, SC_NS);
#ifndef CONCISE
                cout << dec << int(sc_time_stamp().value()) << " ns " << name()
                                                                                                                                            << " current slot " << SlotManager::readPositive()
                << "required slot " << id << " waiting time " << delay
                << hex << setfill('0') << setw(4) << trans->get_address()
                << " phase " << phase << " request num " << dec
                << req_itr->r_id << "residue time is " << residue_time
                << " consumed time " << consumedTime << endl;
#endif
                // Timing annotation models processing time of initiator prior to call

                //assert (sc_delta_count()==0);
                // Non-blocking transport call on the forward path
                timeSlot.notify(*trans, phase, delay);

                // fw_slot_map[trans] = id;

            }
            //}
            //        else
            //        {
            //            cout << "gen_time is " << req_itr-> gen_time << "current time is " << currentTime << "residue time is " << residue_time
            //            << " single try " << singleTry_time<<endl;
            //            delete req_itr;
            //            pending_req[req_id] = 0;
            //            pending_req.erase(req_id);
            //
            //        }
            //*req_itr =0;
            request_queue.pop_front();
        }
    }
}


void SimpleResource_TDM::sending_out(tlm::tlm_generic_payload& trans,
        const tlm::tlm_phase& phase) {
    sc_core::sc_time delay;
    tlm::tlm_sync_enum status;
    tlm::tlm_phase _phase;
    request_monitor* req_itr = new request_monitor;
    Probe_extension *extension;
    trans.get_extension(extension);
    int req_id = extension->r_id;
    req_itr = pending_req.find(req_id)->second;
    bool pass =false;
    int currentSlot =SlotManager::readPositive();
#ifndef CONCISE
    cout << dec << int(sc_time_stamp().value()) << " ns " << name() << " req_id " << dec<<req_id
            << " trans test " << hex << setfill('0') << setw(4)<<trans.get_address() << " phase " << phase<< endl;
#endif
    int currentTime = int(sc_time_stamp().value());

    if (phase == tlm::BEGIN_REQ) {
        assert(pending_req.find(req_id) != pending_req.end());
        if (req_itr->status == Generated) {
            req_itr->send_time = currentTime;
            req_itr->status = Sendout;
            req_itr->retry_time = currentTime;

        }

        if (highwayMonitor->isAvailable(0))
            //if (highwayMonitor->allocate(&trans,0,currentSlot))
        {
            //            cout << dec << int(sc_time_stamp().value()) << " ns " << name() << " req_id " << dec<<req_id
            //                        << " trans" <<trans.get_address() << " get a queue" <<endl;

            // highwayMonitor->configure(&trans,0);
            extension->haveQueue = true;
        }

        pass=true;
        // if (free_slots[SlotManager::readPositive()]){
        occupied_slot=currentSlot;
        // }else
        //
        //send_time[&trans] = currentTime;
    } else if (phase == dataTransfer) {



        if ( !req_itr->busyIngetQueue && !req_itr->getQueueSucceed ) {
            if (highwayMonitor->isAvailable(0) && req_itr->LT_req > (req_itr->amount/4)) {
            req_itr->busyIngetQueue = true;
            extension->tryCheckQueue = true;
            extension->haveQueue = true;
            }
            else
            {
                req_itr->busyIngetQueue = true;
                extension->tryCheckQueue = true;
                extension->haveQueue = false;
		extension->lastCheck = true;
            }
        } else
        {
            extension->tryCheckQueue = false;
        }




        if ( !free_slots[currentSlot]) {
            pass=true;

            //occupied_slot = -1;
        } else {
            assert (highwayMonitor->getTheQueue(&trans));
            if (occupied_slot != currentSlot) {
                pass=true;

            }else
            {
                pass=false;
                occupied_slot = -1;
            }

        }
        if (pass == true) {
            req_itr->LT_req --;
//            cout << dec << int(sc_time_stamp().value()) << " ns " << name()
//                                                    << hex << setfill('0') << setw(4) << trans.get_address()  <<
//                                                    " remain liftime " << req_itr->LT_req <<endl;
            if (highwayMonitor->getTheQueue(&trans) && extension->confirmWithQueue) {
                highwayMonitor->sendAFlit(&trans);

                if (highwayMonitor->isEmpty(&trans)){
                    tlm::tlm_phase fwphase =dataTransfer;
                    delay = sc_time(1, SC_NS);
                    m_peq.notify(trans, fwphase, delay);
                }
            }
        }
        // cout << " pass is " << pass <<endl;

    } else if (phase == cancelDownwardLinks) {

        pass = true;
        free_slots[currentSlot] = true;
        occupied_slot=currentSlot;
    }


    if (pass) {


#ifndef CONCISE
        cout << dec << int(sc_time_stamp().value()) << " ns " << name() << "send  a "
                << hex << setfill('0') << setw(4) << trans.get_address() << " phase "
                << phase << " request num " << dec << req_id << " curSLot " <<  SlotManager::readPositive()<< " try check " << extension->tryCheckQueue << endl;
#endif

        _phase = phase;
        delay = sc_time(0, SC_NS);

        status = (*send_socket[0])->nb_transport_fw(trans, _phase, delay);
    }

}

void SimpleResource_TDM::requestGenerate_process() {

    if (counter > 0) {
        int currentTime = int(sc_time_stamp().value());
        request_monitor *req = new request_monitor;
        //req= req->allocate();
        req->r_id = counter;
        req->col = col;
        req->row = row;
        req->gen_time = currentTime;
        req->THG_req = _throughput;
        req->LT_req = _lifetime;
        req->amount = _lifetime;
        req->status = Generated;
        req->num_succeeds = 0;
        req->close_selection = 0;
        req->selected_trans = NULL;
        req->close_time = 0;
        req->busyIngetQueue = false;
        req->getQueueSucceed =false;
        req->getQueueFailed = false;
        req->higwayEffort = 0;
        request_queue.push_back(counter);
        pending_req[counter] = req;
        req->distance = std::abs(col - int(_node_col))
        + std::abs(row - int(_node_row));

        //#ifndef CONCISE
        cout << dec << currentTime << " ns " << name() << " [gen] " << hex
                << setfill('0') << setw(1) << row << col << _node_row << _node_col
                << " number " << dec << counter << " lf " << _lifetime << " th "
                << _throughput << endl;
        //#endif
        //counter++;
        next_request_event.notify();
    }

    if (infile >> readtime >> col >> row >> _lifetime >> _throughput) {

        sc_time now;
        sc_time when;
        now = sc_time_stamp();
        when = sc_time(readtime, SC_NS);
        next_read.notify(when - now);

        counter++;

    } else
        return;

}



void SimpleResource_TDM::peq_cb_until_success(tlm::tlm_generic_payload& trans,
        const tlm::tlm_phase& phase) {
    sc_core::sc_time fwdelay;
    tlm::tlm_phase fwphase;
    Probe_extension *extension;
    trans.get_extension(extension);
    // cout<< sc_time_stamp() << " " << name()<< "phase " <<phase <<endl;
    int req_id = extension->r_id;
    request_monitor* temp;
    temp = pending_req.find(req_id)->second;
    assert(pending_req.find(req_id)!=pending_req.end());
#ifndef CONCISE
    cout << sc_time_stamp() << " " << name() << " begin " << phase
            << " trans " << hex << setfill('0') << setw(4)
            << trans.get_address() << " req_id " << req_id << "copies"
            << temp->_copies << " has queue " << extension->confirmWithQueue <<extension->confirmQueueInTransfer<<endl;
#endif
    int bwId;
    if (phase == cancelDueSameProbe || phase == cancelDueContention) {
        int req_id = extension->r_id;
        request_monitor* temp;
        temp = pending_req.find(req_id)->second;
        assert(pending_req.find(req_id)!=pending_req.end());
        bwId = extension->src_slotId;
        int currentTime = int(sc_time_stamp().value());
        int consumedTime = currentTime - temp->gen_time;
        // int singleTry_time = temp->distance * 3 + 8 + SLOTDEPTH * SLOTWIDTH;
        //int residue_time = temp->LT_req - consumedTime;

        if (highwayMonitor->getTheQueue(&trans)){
            highwayMonitor->release(&trans);
            extension->haveQueue = false;
        }

        if (temp->status == Sendout) {
            fwphase = internal_Retry;

            // retry[bwId]++;
            //int randRetrytime = int(
            //        rand() % (retry[bwId]* COL+16)   )+6 + 1; //retry time is random in 2*distance ,I think it should not be zero
            int randRetrytime = rand() % 4; //retry time is random in 2*distance ,I think it should not be zero
            fwdelay = sc_time(randRetrytime, SC_NS);
            //fw_port_map.erase(&trans);
            m_peq.notify(trans, fwphase, fwdelay);
        } else //if (m_trans_req.find(&trans)->second->status == PatialSucceed)
        {
            bwId = extension->src_slotId;
            request_in_progress[bwId] = 0;
            //  int currentTime = int(sc_time_stamp().value());


#ifndef CONCISE
            cout << sc_time_stamp() << " " << name() << " begin " << phase
                    << " trans " << hex << setfill('0') << setw(4)
                    << trans.get_address() << " cancel " << "occupyied slot is " << bwId << endl;
#endif
            // fw_slot_map.erase(&trans);
            fwphase = internal_Cancel;

            temp->release();
            if (temp->_copies == 0) {
                //            cout << dec << currentTime << " ns " << name() << " [failed] " << phase
                //                           << hex << " trans " << trans.get_address() << " id " << dec
                //                           << temp->r_id << " num_succs " << temp->num_succeeds
                //                           << " num_req " << ceil(temp->THG_req) << " amount " << 0
                //                           << " setup " << currentTime - temp->send_time << " transfer "
                //                           << 0 << " total " << currentTime - temp->gen_time
                //                           << " retry_times " << retry[bwId] << endl;
                //      cout << "wrongly begin to delete suspections" <<endl;
                assert(temp->_copies==0);
                delete temp;

                //      cout << "finish delete suspections" <<endl;
                pending_req[req_id] = 0;
                pending_req.erase(req_id);
            }

            //m_trans_req.erase(&trans);
            trans.free_all_extensions();
            trans.release();

            //fwdelay = sc_time(1, SC_NS);
            progress_lock.post();
            end_request_event.notify(1, SC_NS);
            // m_peq.notify(trans, fwphase, fwdelay);

        }
        //(*send_socket[bwId])->nb_transport_fw(trans, fwphase, fwdelay);
    } else if (phase == dataTransfer) {
        fwphase = cancelDownwardLinks;

        bwId = extension->src_slotId;
        int wait = SlotManager::pos_waitingTime(bwId);

        if (wait == 0)
            wait = SLOTDEPTH * SLOTWIDTH;

        fwdelay = sc_time(wait, SC_NS);
        timeSlot.notify(trans, fwphase, fwdelay);

#ifndef CONCISE
        cout << sc_time_stamp() << " " << name() << " begin " << fwphase
                << " trans " << hex << setfill('0') << setw(4)
                << trans.get_address() << " slot " << bwId << " wait " << wait
                << endl;
#endif

        request_in_progress[bwId] = 0;
        //retry[bwId] = 0;
        // fw_slot_map.erase(&trans);

        fwphase = internal_Cancel;
        fwdelay = sc_time(1, SC_NS);
        m_peq.notify(trans, fwphase, fwdelay);
    } else if (phase == internal_Cancel) {
        /**erase every thing of old transaction here*/
        //bwId = fw_port_map.find(&trans)->second;
        //request_in_progress->release();
        int req_id = extension->r_id;
        request_monitor* temp;
        temp = pending_req.find(req_id)->second;
        assert(pending_req.find(req_id)!=pending_req.end());
        bwId = extension->src_slotId;


        if (temp->status == PatialSucceed) {
            temp->status = FullySucceed;

        }

        //request_monitor* temp=m_trans_req.find(&trans)->second;
        temp->release();
        if (temp->_copies == 0) {
            //      cout << "begin to delete suspections "<< temp->_copies<<endl;
            int currentTime = int(sc_time_stamp().value());
            cout << dec << currentTime << " ns " << name() << " [request] " << phase
                    << hex << " trans " << temp->row << temp->col <<  dec
                    << " num_succs " << temp->num_succeeds
                    << " num_req " << ceil(temp->THG_req) << " amount "
                    << ceil(temp->amount * temp->THG_req) << " setup "
                    << temp->succeed_time - temp->send_time << " transfer "
                    << currentTime - temp->succeed_time << " total "
                    << currentTime - temp->gen_time << " retry_times "
                    << retry[bwId] << " has queue " << extension->confirmWithQueue << " highway_effort " <<temp->higwayEffort<<endl;
            assert(temp->_copies==0);
            delete temp;
            pending_req[req_id] = 0;
            pending_req.erase(req_id);
            retry[bwId] = 0;
            //      cout << "finish delete suspections" <<endl;
        }
        if (highwayMonitor->getTheQueue(&trans)) {
            highwayMonitor->release(&trans);
#ifndef CONCISE
            cout << dec << sc_time_stamp().value() << " ns " << name() << " trans " << trans.get_address()<<"released a queue "  << endl;
#endif
        }

        //m_trans_req.erase(&trans);
        progress_lock.post();
        end_request_event.notify(1, SC_NS);

    } else if (phase == succeed) {
        int req_id = extension->r_id;
        request_monitor* temp;
        temp = pending_req.find(req_id)->second;
        assert(pending_req.find(req_id)!=pending_req.end());

        //assert (m_trans_req.find(&trans) != m_trans_req.end());
        if (temp->status == Sendout) {

            temp->status = PatialSucceed;
            temp->succeed_time = sc_time_stamp().value();
            temp->close_time = temp->succeed_time + 2*SLOTDEPTH-1;

        }
#ifndef CONCISE
        cout << sc_time_stamp() << " " << name() << " begin " << phase << " trans "
                << hex << setfill('0') << setw(4) << trans.get_address()
                << "succeed waiting time " << temp->succeed_time << endl;
#endif
        //request_monitor * temp =m_trans_req.find(&trans)->second;

        if (extension->confirmWithQueue ==0) {
            if (highwayMonitor->getTheQueue(&trans)) {          // these code should not be excecuted;
                highwayMonitor->release(&trans);
                assert (false);
            }

        }else
        {


            if ( highwayMonitor->allocate(&trans,0)) {
                highwayMonitor->configure(&trans,0);
            }
            else {
                extension->confirmWithQueue =false;
            }

        }


        //m_req_trans.insert(
        //      pair<request_monitor*, tlm::tlm_generic_payload *>(temp,
        //             &trans));
        //  if (temp->num_succeeds == ceil(temp->THG_req)) {
        //     int delay = temp->succeed_time - sc_time_stamp().value();
        //      assert(delay>=1);
        if (temp->close_selection) {
            fwphase = dataTransfer;

            fwdelay = sc_time(2, SC_NS);
            //                       }

#ifndef CONCISE
            cout << sc_time_stamp() << " " << name() << " begin " << fwphase
                    << " trans " << hex << setfill('0') << setw(4)
                    << trans.get_address() << endl;
#endif


            m_peq.notify(trans, fwphase, fwdelay);
        }
        else if (extension->confirmWithQueue) {

            highwayMonitor->presetQueue(&trans, temp->LT_req );
            next_send.notify(1,SC_NS);
            temp->higwayEffort = temp->LT_req;
            temp->selected_trans= &trans;
            temp->close_selection = true;
            temp->num_succeeds++;
            temp->getQueueSucceed = true;
            temp->succeed_time = sc_time_stamp().value();

        }  else if (!extension->confirmWithQueue){
            if (highwayMonitor->getTheQueue(&trans)) {
                highwayMonitor->release(&trans);
            }

            //  if (sc_time_stamp().value() < temp->close_time) {
            temp->num_succeeds++;
            temp->selected_trans= &trans;
            fwphase = wait_Transfer;
            temp->close_selection = true;
            fwdelay = sc_time (1, SC_NS);
            temp->succeed_time = sc_time_stamp().value();
        
            m_peq.notify(trans, fwphase, fwdelay);
	
	
        }



    } else if (phase == internal_Retry) {
        int req_id = extension->r_id;
        request_monitor* temp;
        temp = pending_req.find(req_id)->second;
        assert(pending_req.find(req_id)!=pending_req.end());
        if (temp->status == Sendout) {
            fwphase = tlm::BEGIN_REQ;
            fwdelay = sc_time(0, SC_NS);
            bwId = extension->src_slotId;
            retry[bwId]++;

            //        int bwid = int(rand() % CHANNEL_PER_DIRECTION);
            // int bwid = bwId;
            // fw_port_map.erase(&trans);
            // fw_port_map[&trans] = bwid;
            int wait = SlotManager::pos_waitingTime(bwId);

            if (wait == 0)
                wait = SLOTDEPTH * SLOTWIDTH;
            temp->retry_time = sc_time_stamp().value() + wait;
#ifndef CONCISE
            cout << sc_time_stamp() << " " << name() << " begin retry " << " trans "
                    << hex << setfill('0') << setw(4) << trans.get_address()
                    << " slot " << bwId << " wait " << wait << endl;
#endif
            fwdelay = sc_time(wait, SC_NS);
            timeSlot.notify(trans, fwphase, fwdelay);
            //  (*send_socket[bwId])->nb_transport_fw(trans, fwphase, fwdelay);
        } else //if ( m_trans_req.find(&trans)->second->status == PatialSucceed)
        {
            bwId = extension->src_slotId;
            request_in_progress[bwId] = 0;
#ifndef CONCISE
            cout << sc_time_stamp() << " " << name() << " begin " << phase
                    << " trans " << hex << setfill('0') << setw(4)
                    << trans.get_address() << " partial cancel retry" << endl;
#endif
            //fw_slot_map.erase(&trans);
            fwphase = internal_Cancel;
            //delete m_trans_req.find(&trans)->second;

            temp->release();
            if (temp->_copies == 0) {
                //      cout << "wrongly begin to delete suspections" <<endl;
                assert(temp->_copies==0);
                delete temp;
                pending_req[req_id] = 0;
                pending_req.erase(req_id);
                //      cout << "finish delete suspections" <<endl;
            }

            // m_trans_req.erase(&trans);
            trans.free_all_extensions();
            trans.release();
            progress_lock.post();
            end_request_event.notify(1, SC_NS);
        }

    } else if (phase == wait_Transfer) {
        int req_id = extension->r_id;
        request_monitor* temp;
        temp = pending_req.find(req_id)->second;
        assert(pending_req.find(req_id)!=pending_req.end());

        //        int residueTime = int(sc_time_stamp().value()) - temp->retry_time;
        //        int backlog = (residueTime % SLOTWIDTH) / SLOTDEPTH;
        //        backlog = backlog * SLOTWIDTH;


        bwId = extension->src_slotId;

        // if (!extension->haveQueue) {

        //if (temp->LT_req - backlog >0 )
        // {
        //        int duriation;
        //
        //        duriation = ceil(temp->LT_req)/ ( SLOTWIDTH);
        //        //duriation= ceil (ceil(temp->LT_req-backlog) *ceil( temp->THG_req ) )/ (temp->num_succeeds * SLOTWIDTH);
        //        duriation = (duriation + 1) * SLOTDEPTH * SLOTWIDTH;
        int wait = SlotManager::pos_waitingTime(bwId);

        if (wait == 0)
            wait = SLOTDEPTH * SLOTWIDTH;



        if (temp->LT_req ==0) {
            fwdelay = sc_time(1, SC_NS);
            fwphase = dataTransfer;
            m_peq.notify(trans, fwphase, fwdelay);
        }
        else if (!temp->getQueueSucceed){
            fwphase = dataTransfer;
            fwdelay = sc_time(wait, SC_NS);
            timeSlot.notify(trans, fwphase, fwdelay);


            fwphase = wait_Transfer;
            fwdelay = sc_time (wait+SLOTDEPTH -1, SC_NS );   // why +slot
            m_peq.notify (trans, fwphase, fwdelay);



        } else if (temp->getQueueSucceed) {

            next_send.notify(1,SC_NS);
            temp->higwayEffort = temp->LT_req;
            temp->selected_trans= &trans;
            temp->close_selection = true;
            temp->num_succeeds++;
            highwayMonitor->configure(&trans, 0);
            highwayMonitor->presetQueue(&trans, temp->LT_req );//make queue start
            if (temp->LT_req < temp->amount/16) {

               cerr << " the highway allocation takes too much time!" <<endl;
            }
            extension->confirmWithQueue = true;
        }





    }else if (phase == bookAqueue) {   //book a queue during data transfer time
        int req_id = extension->r_id;
        request_monitor* temp;
        temp = pending_req.find(req_id)->second;
        assert(pending_req.find(req_id)!=pending_req.end());
        int slot = SlotManager::readnegtive();
        slot = (slot - 1 + SLOTDEPTH) % SLOTDEPTH;

        temp->busyIngetQueue = false;
        assert (!extension->confirmWithQueue);

        if (extension->confirmQueueInTransfer ==0) {
            //                if (queueOfAllInputs->getTheQueue(trans)) {
            //                    queueOfAllInputs->release(trans);
            //                }
            //assert (false);

            temp->getQueueSucceed = false;

        }else
        {

            if ( highwayMonitor->allocate(&trans,0,slot)) {
                //                std::cout << sc_time_stamp() << " " << name()
                //                           << phase << " trans " << hex << setfill('0') << setw(4)
                //                           << trans.get_address() <<
                //


                temp->getQueueSucceed = true;
            }
            else {
                extension->confirmQueueInTransfer =0;
                temp->getQueueSucceed = false;
            }

        }
        //if (highwayMonitor->)


    }



    else {
        cout << " Error: no such phase " << phase << endl;
    }

}


void SimpleResource_TDM::highwaySend_process(){

    sc_core::sc_time fwdelay;
    tlm::tlm_phase fwphase;
    //Probe_extension *extension;
    //trans.get_extension(extension);
    //  int  currentSlot = SlotManager::readPositive();
    vector <tlm::tlm_generic_payload * > transes;
    tlm::tlm_generic_payload * trans;
    transes.resize(1,NULL);
#ifndef CONCISE
    cout<<sc_time_stamp() << " " << name() << " begin to arbitrate " <<endl;
#endif
    vector < bool > temp;




    temp.resize(1,free_slots[Slot.readPositive()]);
    transes = highwayMonitor->weighted_arbitrate(temp);
    if (occupied_slot != Slot.readPositive()   ) {
        occupied_slot = -1;
    }

    // for (int i=0; i <HIGHWAYOFEACHDIR; i++)
    if (transes[0]!=NULL) {

        fwphase=dataTransfer;
        fwdelay = sc_time(0, SC_NS);
        trans=transes[0];

//        Probe_extension *extension;
//        trans->get_extension(extension);
//
//        assert (extension->confirmWithQueue);

#ifndef CONCISE
        cout <<"trans " <<transes[0]->get_address()<< "arbitrate success, remain flits " << dec << highwayMonitor->getTheQueue(trans)->currentPointer <<endl;
#endif
        timeSlot.notify(*trans, fwphase, fwdelay);

        //cout << transes[0]->get_address() << " will be delivered " <<endl;

    }



    if (!highwayMonitor->allIsEmpty()){
        next_send.notify(1,SC_NS);
    }



}



void SimpleResource_TDM::sending_back(tlm::tlm_generic_payload& trans,
        const tlm::tlm_phase& phase) {
    //   std::cout << sc_time_stamp() << " " << name()<< endl;
    Probe_extension *extension;
    trans.get_extension(extension);
    int id = extension->temp_inSlotId;
    sc_core::sc_time bwDelay = sc_time(0, SC_NS);
    tlm::tlm_phase bwPhase = phase;
#ifndef CONCISE
    std::cout << sc_time_stamp() << " " << name() << " send back to input slot " << id
            << " " << phase << " trans " << hex << setfill('0') << setw(4)
            << trans.get_address() << std::endl;

#endif
    if ( phase != cancelDownwardLinks) {

        recieve_socket[id]->nb_transport_bw(trans, bwPhase, bwDelay);

    } else
    {
        trans.free_all_extensions();
        trans.release();
    }

}

tlm::tlm_sync_enum SimpleResource_TDM::recieve_nb_transport_fw(int id,
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,

        sc_core::sc_time& delay) {
    tlm::tlm_phase bwPhase;
    tlm::tlm_generic_payload * bwtrans;
    sc_core::sc_time bwDelay = sc_time(1, SC_NS);

    Probe_extension *extension;
    trans.get_extension(extension);
    extension->temp_inSlotId = id;
#ifndef CONCISE
    std::cout << sc_time_stamp() << " " << name() << " in input " << id << " "
            << phase << " trans " << hex << setfill('0') << setw(4)
            << trans.get_address() << dec << SlotManager::PosToNegSlot() << " " << "source slot"<<extension->src_slotId<< endl;
    //  << bwDelay << endl;
#endif

    if (phase == tlm::BEGIN_REQ) {
        bwPhase = succeed;
        bwtrans = &trans;
        int currentNegSlot = SlotManager::readnegtive();
        currentNegSlot = (currentNegSlot - 1 + SLOTDEPTH) % SLOTDEPTH;
        // cout << " curr" <<currentNegSlot << endl;
        bwDelay = sc_time(SlotManager::pos_waitingTime(currentNegSlot), SC_NS);
        if (bwDelay.value() == 0)
            bwDelay = sc_time(SLOTDEPTH * SLOTWIDTH, SC_NS);
        // bw_content[currentPosSLot][bwId]=multiPhase;



        bw_transfer.notify(*bwtrans, bwPhase, bwDelay);
        extension->confirmWithQueue=extension->haveQueue;
        // cout << "laal" <<endl;

        // (*send_socket[1])->nb_transport_fw(trans, phase, delay);
    } else if (phase == cancelDownwardLinks) {

        bwPhase = cancelDownwardLinks;
        bwtrans = &trans;
        bwDelay = sc_time(SLOTDEPTH,SC_NS);
        bw_transfer.notify(*bwtrans, bwPhase, bwDelay);

    } else if (phase == dataTransfer) {
        if (extension->confirmWithQueue) {
            bwPhase = haveCredit;
            bwtrans = &trans;
            bwDelay = sc_time(1,SC_NS);
            bw_transfer.notify(*bwtrans, bwPhase, bwDelay);
        }
        else if (extension->tryCheckQueue && !extension->lastCheck) {
            bwPhase = bookAqueue;
            bwtrans = &trans;
            int currentNegSlot = SlotManager::readnegtive();
            currentNegSlot = (currentNegSlot - 1 + SLOTDEPTH) % SLOTDEPTH;
            // cout << " curr" <<currentNegSlot << endl;
            bwDelay = sc_time(SlotManager::pos_waitingTime(currentNegSlot), SC_NS);
            if (bwDelay.value() == 0)
                bwDelay = sc_time(SLOTDEPTH * SLOTWIDTH, SC_NS);
            bw_transfer.notify(*bwtrans, bwPhase, bwDelay);
            extension->confirmQueueInTransfer = extension->haveQueue;

        }

    }

    return tlm::TLM_ACCEPTED;

}

void SimpleResource_TDM::check_transaction(tlm::tlm_generic_payload& trans) {
    if (trans.is_response_error()) {
        char txt[100];
        sprintf(txt, "Transaction returned with error, response status = %s",
                trans.get_response_string().c_str());
        SC_REPORT_ERROR("TLM-2", txt);
    }

    trans.release();
}

tlm::tlm_sync_enum SimpleResource_TDM::send_nb_transport_bw(int id,
        tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
        sc_core::sc_time& delay) {
    int slot = SlotManager::readnegtive();
    slot = (slot - 1 + SLOTDEPTH) % SLOTDEPTH;
#ifndef CONCISE
    std::cout << sc_time_stamp() << " " << name() << " in input " << id << " "
            << phase << " trans " << hex << setfill('0') << setw(4)
            << trans.get_address() << dec << " slot " << slot << endl;
#endif
    assert(request_in_progress[slot]== &trans || phase == haveCredit);
    // The timing annotation must be honored
    if (phase == cancelDueSameProbe || phase == cancelDueContention)
        delay = sc_core::sc_time(1, SC_NS);
    else if (phase == succeed) {
        delay = sc_core::sc_time(1, SC_NS);

        free_slots[slot]=false;

    } else if (phase == bookAqueue) {
        m_peq.notify(trans, phase);
    }

    //cout << name() << " "<< hex<< request_in_progress[slot]->get_address() << endl;
    if ( phase == cancelDueSameProbe || phase == cancelDueContention || phase == succeed ) {
        m_peq.notify(trans, phase, delay);
    }
    else if (phase == haveCredit) {
        if (highwayMonitor->getTheQueue(&trans))
            highwayMonitor->IncreNextToken(&trans);
    }
    //    else if (phase == bookAqueue) {
    //
    //
    //    }

    return tlm::TLM_ACCEPTED;
}
