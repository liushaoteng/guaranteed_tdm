#include "../core_of_single_router/commonheader.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

void test_sc_uint(){
sc_uint <4> _dest_col = 15; // dest address col
            sc_uint <4> _dest_row = 15; // dest address row
            sc_uint <4> _src_col=15;
            sc_uint <4> _src_row=15;
            sc_uint <16> _addr=(_dest_col,_dest_row,_src_col,_src_row) ;
            unsigned int addr=_addr.to_int();
            cout <<_addr.to_string(SC_BIN)<<endl;
}
