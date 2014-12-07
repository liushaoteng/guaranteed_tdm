#ifndef DUMMYMODULE_H_
#define DUMMYMODULE_H_
#include "commonheader.h"
#include "MemManager.h"
#include "networkdef.h"

namespace PPB {
class DummyModule: sc_core::sc_module {
public:
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::multi_passthrough_initiator_socket<DummyModule, 32,
            Probe_protocol_types> send_socket;
    tlm_utils::multi_passthrough_target_socket<DummyModule, 32,
            Probe_protocol_types> recieve_socket;

    DummyModule(sc_core::sc_module_name _n, int _row, int _col) :
            sc_module(_n), send_socket("send"), recieve_socket("receive"), _node_col(
                    _col), _node_row(_row) {

        recieve_socket.register_nb_transport_fw(this,
                &DummyModule::recieve_nb_transport_fw);
        send_socket.register_nb_transport_bw(this,
                &DummyModule::send_nb_transport_bw);
//        for (int i = 0; i < CHANNEL_PER_DIRECTION; i++) {
//            char txt[20];
//            sprintf(txt, "send_socket_%d", i);
//            send_socket[i] = new tlm_utils::simple_initiator_socket_tagged<
//                    DummyModule, 32, Probe_protocol_types>(txt);
//            send_socket[i]->register_nb_transport_bw(this,
//                    &DummyModule::send_nb_transport_bw, i);
//
//        }
        //init_socket.register_nb_transport_bw(          this, &Bus::nb_transport_bw);

    }
    //  SC_HAS_PROCESS(DummyModule);

    // TLM-2 backward non-blocking transport method

    virtual tlm::tlm_sync_enum send_nb_transport_bw(int id,
            tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
            sc_core::sc_time& delay) {
    }

    virtual tlm::tlm_sync_enum recieve_nb_transport_fw(int id,
            tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,
            sc_core::sc_time& delay) {
    }
    // Payload event queue callback to handle transactions from target
    // Transaction could have arrived through return path or backward path

private:
    const unsigned int _node_col;
    const unsigned int _node_row;
};

}
#endif
