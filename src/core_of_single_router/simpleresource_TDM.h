#ifndef SIMPLERESOURCE_H_
#define SIMPLERESOURCE_H_
#include "commonheader.h"
#include "MemManager.h"
#include "networkdef.h"
#include "SlotTable.h"
#include <fstream>
#include <list>
#include "Highways.h"
#include <set>
// using namespace sc_core;
// using namespace std;
// using namespace sc_dt;




namespace PPB {
class SimpleResource_TDM: sc_core::sc_module {
public:
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket_tagged<SimpleResource_TDM, 32,
            Probe_protocol_types>* send_socket[RESOURCE_CHANNEL_NUM];
    tlm_utils::multi_passthrough_target_socket<SimpleResource_TDM, 32,
            Probe_protocol_types> recieve_socket;
    typedef std::pair<int, int> ADDRESS;
    typedef std::pair<ADDRESS, int> REQUEST;

    SimpleResource_TDM(sc_core::sc_module_name _n, int _row, int _col) :
            sc_module(_n), recieve_socket("receive"),  m_peq(
                    this, &SimpleResource_TDM::peq_cb_until_success), _node_col(_col), _node_row(
                    _row), progress_lock(SLOTDEPTH), timeSlot(this,&SimpleResource_TDM::sending_out), bw_transfer(this,&SimpleResource_TDM::sending_back){
        // Register callbacks for incoming interface method calls

        recieve_socket.register_nb_transport_fw(this,
                &SimpleResource_TDM::recieve_nb_transport_fw);
        //progress_lock = 0;
        for (int i = 0; i < RESOURCE_CHANNEL_NUM; i++) {
            char txt[20];
            sprintf(txt, "send_socket_%d", i);
            send_socket[i] = new tlm_utils::simple_initiator_socket_tagged<
                    SimpleResource_TDM, 32, Probe_protocol_types>(txt);
            send_socket[i]->register_nb_transport_bw(this,
                    &SimpleResource_TDM::send_nb_transport_bw, i);

        }
        std::string name ;
        name= _n;
        highwayMonitor= new Highways(HIGHWAYOFEACHDIR,1000, name);
        char fileName[256];
        //char cache200[200];
        sprintf(fileName, "%s/%02x%02x.txt", path,_node_col, _node_row);
        infile.open(fileName);
        if (!infile) {
            std::cerr << "error: unable to open input file " << fileName
                    << std::endl;

        }
        free_slots.resize(SLOTDEPTH,true);
        //init_socket.register_nb_transport_bw(          this, &Bus::nb_transport_bw);
        //SC_THREAD(simpleSend_process);
        SC_THREAD(standardSend_process_until_success);
        sensitive << next_request_event;

        SC_METHOD(requestGenerate_process);
        sensitive << next_read;

        SC_METHOD(highwaySend_process);
        sensitive << next_send;
        dont_initialize();

        counter = 0;
        //sendtime = 0;
        for (int id =0; id<SLOTDEPTH; id++)
        {retry [id] = 0;
         request_in_progress[id]=0;
        }
        occupied_slot = -1;
        //endtime = 0;
    }
    SC_HAS_PROCESS(SimpleResource_TDM);

   

    // void simpleSend_process();

    void standardSend_process();
    void standardSend_process_until_success();
    // TLM-2 backward non-blocking transport method
    void requestGenerate_process();
    void highwaySend_process();


    virtual tlm::tlm_sync_enum send_nb_transport_bw(int id,
            tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
            sc_core::sc_time& delay);

    virtual tlm::tlm_sync_enum recieve_nb_transport_fw(int id,
            tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
            sc_core::sc_time& delay);
    // Payload event queue callback to handle transactions from target
    // Transaction could have arrived through return path or backward path



    // Called on receiving BEGIN_RESP or TLM_COMPLETED
    void check_transaction(tlm::tlm_generic_payload& trans);

    virtual ~SimpleResource_TDM() {
        for (int i = 0; i < RESOURCE_CHANNEL_NUM; i++) {
            delete send_socket[i];
        }
        delete highwayMonitor;
    }

    gp_mm m_mm;
    int priority[2];
    char request;
    tlm::tlm_generic_payload* request_in_progress[SLOTDEPTH];
    sc_core::sc_event end_request_event;
    // sc_core::sc_event new_request_event;
    sc_core::sc_event next_request_event;
    sc_core::sc_event next_read;
    sc_core::sc_event next_send;
    tlm_utils::peq_with_cb_and_phase<SimpleResource_TDM> m_peq;
    tlm_utils::peq_with_cb_and_phase<SimpleResource_TDM> timeSlot;
    tlm_utils::peq_with_cb_and_phase<SimpleResource_TDM> bw_transfer;
   // std::map<tlm::tlm_generic_payload *, int> fw_slot_map;   // trans - > slot
    std::list<int> request_queue;

   // std::map<tlm::tlm_generic_payload *, int> send_time;
   // std::map<tlm::tlm_generic_payload *, int> gen_time;
   // std::map<tlm::tlm_generic_payload *, int> m_trans_req;    // trans -> request id;
   // std::multimap<request_monitor*,tlm::tlm_generic_payload *> m_req_trans;
    std::map<int,request_monitor*> pending_req; 	

private:
    const unsigned int _node_col;
    const unsigned int _node_row;
    sc_core::sc_semaphore progress_lock;
    std::ifstream infile;
    int readtime, col, row, counter;
    double _throughput;
    int _lifetime;
    //int sendtime, endtime;
    int retry [SLOTDEPTH];
    Highways *highwayMonitor;

    std::vector <bool> free_slots;
       int occupied_slot;



    void sending_out(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);
    void sending_back(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);
    void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);
    void peq_cb_until_success(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);
    // int _next_req_id;
};

}
#endif
