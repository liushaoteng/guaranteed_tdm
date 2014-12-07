/*
 * waterfall.cpp
 *
 *  Created on: Jun 20, 2012
 *      Author: lsteng
 */
#include "Waterfall.h"
#define alloc_test


using namespace PPB;
using namespace std;



void Waterfall::Allocate(int & currentPosSLot) {
    std::set<int>::iterator req_itr;
#ifndef alloc_test
    *gWatchOut<<GetSimTime()<<"num of ports " <<_num_ports <<endl;
    *gWatchOut<<GetSimTime()<<"_avail_output_ports is "<<endl;
    for (int i=0; i<_num_ports*_gNumVCs; i++)
    {
        *gWatchOut <<i <<" " << _availoutput[i] <<endl;
    }
    *gWatchOut<<GetSimTime()<<" current positions is "<<endl;
    for (int i=0; i<_num_ports; i++)
    {
        *gWatchOut <<i <<" " << _current_position[i] <<endl;
    }
    *gWatchOut<<GetSimTime()<<" number of requests for one drctn is "<<endl;
    for (int i=0; i<_num_ports; i++)
    {
        *gWatchOut <<i <<" " << _num_requests_for_one_drctn[i] <<endl;
    }
    *gWatchOut<<GetSimTime()<<" output sets is "<<endl;
    for (int outputdrctn = 0; outputdrctn < _num_ports; outputdrctn++) {
        for (req_itr = outputs_sets[outputdrctn].begin();
                req_itr != outputs_sets[outputdrctn].end(); req_itr++) {
            *gWatchOut <<outputdrctn <<" " << *req_itr <<endl;
        }
    }
#endif

    inputVec::iterator itr;
    filter(currentPosSLot);
    for (itr = _newTrans.begin(); itr!= _newTrans.end(); itr++)
    {
        int in =itr->second.inputChannel.id;
        for (int i=0; i<2; i++)
        {
            int out = itr->second.aimingDirections[i];

            if ( out != Nodrctn )
                //outputs_sets[out].insert (in);
            {
                bool haveQueue = itr->second.ownAQueue;
                //itr->first->get_extension(extension);
                if (haveQueue) {
                    _output_arb[out]->AddRequest(in,in,1);
                } else
                {
                    _output_arb[out]->AddRequest(in,in,0);
                }
            }

        }
        // cout << "input_arb" << in_id << "add output"<<resourceId <<endl;

    }

    for (int i=0; i<NUM_DIRECTIONS; i++)
    {
        if (_output_arb[i]->_num_reqs>0){
            int label = -1;
            const int input = _output_arb[i]->Arbitrate(&label, NULL);
            assert(input > -1);
            //register connection table
            _in_out_match[input]=i;
            _out_in_match[i]=input;
            //
            //

            _output_arb[i]->UpdateState() ;
            //  cout << "output_arb" << i << "grant input"<<input << "its position is "<<_input_arb[input]->current_pointer()<<endl;
            _output_arb[i]->Clear();
        }

    }

//    for (int outputdrctn = 0; outputdrctn < _outputs; outputdrctn++)
//    {
//        //int drctn_vc_begin = outputdrctn * _gNumVCs;
//        //int drctn_vc_end = (outputdrctn + 1) * _gNumVCs;
//
//        int start;
//        int input;
//        //int first_input;
//
//        if (outputs_sets[outputdrctn].empty())
//            continue;
//
//        int next = _current_position[outputdrctn] + 1;
//
//        if (next >= _inputs)
//            next = 0;
//
//        //start=0;
//        for (req_itr = outputs_sets[outputdrctn].begin();
//                req_itr != outputs_sets[outputdrctn].end(); req_itr++) {
//            if (*req_itr >= next) {
//                start = *req_itr;
//                //  _current_position[outputdrctn] = start;                           // is this way correct?
//
//                break;
//            }
//        }
//
//        if (req_itr == outputs_sets[outputdrctn].end()) {
//            req_itr = outputs_sets[outputdrctn].begin();
//            start = *req_itr;
//            //  _current_position[outputdrctn] = start;
//        }
//
//        req_itr = outputs_sets[outputdrctn].find(start);
//
//
//
//        input = *req_itr;
//        _in_out_match[input] = outputdrctn;
//#ifndef alloc_test
//        *gWatchOut<<GetSimTime()<<" out_resource_id is "<<out_resource_id<<endl;
//#endif
//        _out_in_match[outputdrctn] = input;
//
//        _current_position[outputdrctn] = *req_itr;
//
//
//        //            req_itr++;
//        //            if (req_itr == outputs_sets[outputdrctn].end()) {
//        //                req_itr = outputs_sets[outputdrctn].begin();
//        //            }
//
//
//        //      } while (*req_itr != start);
//        outputs_sets[outputdrctn].clear();
//
//
//    }

    for (itr = _newTrans.begin(); itr!= _newTrans.end(); itr++)
    {
        int in=itr->second.inputChannel.id;
        if (_in_out_match[in]!= Nodrctn)
        {
            for (int i=0; i<2; i++)
            {
                int out = itr->second.aimingDirections[i];
                if ( (out != Nodrctn )  && (_out_in_match[out]==in)) {
                   //  cout << "add " << " in " << in << " out " << out <<endl;
                    Channel c;
                    c.id=out;
                    itr->second.outputChannel.push_back(c);
                }
            }
        }
    }
    clear();

}

