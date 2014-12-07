/*
 * TDMRouter.h
 *
 *  Created on: Mar 4, 2013
 *      Author: lsteng
 */

#ifndef TDMROUTER_H_
#define TDMROUTER_H_
#include "commonheader.h"
#include "networkdef.h"

#include "assert.h"
#include "peq_with_get_map.h"
#include "Allocator.h"
#include "SlotTable.h"
#include "Waterfall.h"

#include "Highways.h"

namespace PPB {
class TDM_Router: sc_core::sc_module {
public:
    tlm_utils::simple_target_socket_tagged<TDM_Router, 32, Probe_protocol_types>* in_socket[TOTAL_NUM_CHANNELS];
    tlm_utils::simple_initiator_socket_tagged<TDM_Router, 32, Probe_protocol_types>* out_socket[TOTAL_NUM_CHANNELS];
    typedef std::multimap<tlm::tlm_generic_payload*, ProbeProperty>::iterator NewTrans_itr;
    typedef std::pair<tlm::tlm_generic_payload*, tlm::tlm_phase> PAYLOAD;
    typedef std::pair<int, PAYLOAD> MULTIPAYLOAD;
    TDM_Router(sc_core::sc_module_name _n, int _row, int _col) :
        sc_module(_n),_node_col(_col), _node_row(_row),
        fw_map(this, &TDM_Router::_fw_stage_1),slotTable(SLOTDEPTH, _n){

        // stage_1_counter=0;
        // stage_2_counter=0;
        //in_socket.register_nb_transport_fw(this, &Router::in_nb_transport_fw);
        _name = _n;
        // highOfAllInputs.resize (NUM_DIRECTIONS);
        //slotTable(SLOTDEPTH,_name);
        for (int i = 0; i < TOTAL_NUM_CHANNELS; i++) {
            char txt_out[20];
            char txt_in[20];
            sprintf(txt_out, "out__%d", i);
            sprintf(txt_in, "in_%d", i);
            out_socket[i] = new tlm_utils::simple_initiator_socket_tagged<
                    TDM_Router, 32, Probe_protocol_types>(txt_out);
            out_socket[i]->register_nb_transport_bw(this,
                    &TDM_Router::out_nb_transport_bw, i);
            in_socket[i] = new tlm_utils::simple_target_socket_tagged<TDM_Router,
                    32, Probe_protocol_types>(txt_in);
            in_socket[i]->register_nb_transport_fw(this,
                    &TDM_Router::in_nb_transport_fw, i);



        }
        queueOfAllInputs = new Highways (NUM_DIRECTIONS*HIGHWAYOFEACHDIR, HIGHWAYDEPTH, HIGHWAYOFEACHDIR, 1, _name);

        _routeFunc= & TDM_Router::adaptive;
        allocator = Allocator::NewAllocator (_name, NUM_DIRECTIONS, NUM_DIRECTIONS, newTrans, slotTable, "waterfall");
        occupied_output.resize(NUM_DIRECTIONS,false);
        transForSendInNextCycle.resize (NUM_DIRECTIONS,NULL);
        bw_content_cleanmark.resize(SLOTDEPTH);
        bw_content.resize (SLOTDEPTH);
        for (int i=0; i<SLOTDEPTH; i++)
            //  for (int j=0; j<NUM_DIRECTIONS; j++)
        {

            bw_content_cleanmark[i].resize(NUM_DIRECTIONS,false);
            bw_content[i].resize(NUM_DIRECTIONS,tlm::BEGIN_REQ);

        }


        //        SC_METHOD(_fw_stage_1);
        //        sensitive << fw_map.get_event();
        //        dont_initialize();

        SC_METHOD(_fw_stage_2);
        sensitive << fw_stage2_event;
        dont_initialize();

        SC_METHOD(_fw_stage_3);
                sensitive << fw_stage3_event;
                dont_initialize();

        SC_METHOD(_erase_stage);
        sensitive << erase_event;
        dont_initialize();

        SC_METHOD(_bw_operation);
        sensitive << bw_event;
        dont_initialize();
    }
    SC_HAS_PROCESS(TDM_Router);

    virtual tlm::tlm_sync_enum in_nb_transport_fw(int id,
            tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
            sc_core::sc_time& delay);
    virtual tlm::tlm_sync_enum out_nb_transport_bw(int id,
            tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
            sc_core::sc_time& delay);

    virtual ~TDM_Router() {
        for (int i = 0; i < TOTAL_NUM_CHANNELS; i++) {
            delete in_socket[i];
            delete out_socket[i];
            //delete queueOfAllInputs;
        }
        delete allocator;
        delete queueOfAllInputs;
    }






private:



    const unsigned int _node_col;
    const unsigned int _node_row;
    std::string _name;
    //int stage_1_counter;
    // int stage_2_counter;

    void _fw_stage_1(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);
    void _fw_stage_2();
    void _fw_stage_3();

    void _erase_stage();

    void _bw_operation();
    int cancelMethod_simple(int inId, tlm::tlm_generic_payload * trans, NewTrans_itr itr);

    std::vector <tlm::tlm_generic_payload* > transForSendInNextCycle;
    std::vector <tlm::tlm_generic_payload* > transForSendInCurCycle;

    std::multimap<tlm::tlm_generic_payload*, ProbeProperty> tempProbes;  // for the storage of probes  stage 1
    std::map<tlm::tlm_generic_payload*, ProbeProperty> newTrans;  // for the storage of probes    stage 2
    std::map<tlm::tlm_generic_payload*, ProbeProperty> bookedTrans;   // used after cancel due to same probes; other stages


    sc_core::sc_event fw_stage1_event;
    sc_core::sc_event fw_stage2_event;
     sc_core::sc_event fw_stage3_event;

    sc_core::sc_event erase_event;
    sc_core::sc_event bw_event;

    std::multimap<sc_core::sc_time, MULTIPAYLOAD> bw_map;
    tlm_utils::peq_with_cb_and_phase<TDM_Router> fw_map;
    std::vector <bool> occupied_output;


    //std::map < tlm::tlm_generic_payload *, int> fw_port_map;

    // tlm_utils:: peq_with_get <Router> bw_event_q;
    // tlm_utils:: peq_with_get <Router> fw_event_q;


    typedef void (TDM_Router::* routingFunc)(ProbeProperty * property, Probe_extension* extension);

    routingFunc _routeFunc;


    //Routerstorage storage;
    void getAim(ProbeProperty *property, Probe_extension* extension);
    void adaptive(ProbeProperty * property, Probe_extension* extension);
    void XY(ProbeProperty * property, Probe_extension* extension);


    bool probe_transaction_release(
            std::map<tlm::tlm_generic_payload*, ProbeProperty*>::iterator itr);
    Allocator *allocator;
    SlotTable slotTable;



    std::vector < std::vector<tlm::tlm_phase> > bw_content;
    std::vector < std::vector <bool> > bw_content_cleanmark;

    Highways* queueOfAllInputs;


};
}
#endif /* TDMROUTER_H_ */
