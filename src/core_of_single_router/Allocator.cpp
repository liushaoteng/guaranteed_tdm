/*
 * allocator.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: lsteng
 */

#include "Allocator.h"
#include "Waterfall.h"

using namespace PPB;
using namespace std;

Allocator::~Allocator() {

    // TODO Auto-generated destructor stub
}

bool Allocator::filter(int &currentPosSlot) {
    inputVec::iterator itr;
    for (itr = _newTrans.begin(); itr!= _newTrans.end(); itr++)
    {
        // int in =itr->second.inputChannel.id;
        for (int i=0; i<2; i++)
        {
            int out = itr->second.aimingDirections[i];

            if ( _slotTable.checkValids(currentPosSlot)[out] == false)
            {
                itr->second.aimingDirections[i]=Nodrctn;
            }


        }
        //itr->second.invalid=true;
    }
    return true;
}

void Allocator::clear( ) {

    for (int i=0; i<_inputs; i++)
    {
        _in_out_match[i]=Nodrctn;
        _out_in_match [i]=Nodrctn;
    }
}



void Input_first_allocator::Allocate( int & currentPosSLot) {
    // std::multimap< int,  PPB::ProbeProperty &>::iterator itr;

    inputVec::iterator itr;
    filter(currentPosSLot);
    for (itr = _newTrans.begin(); itr!= _newTrans.end(); itr++)
    {
        int in =itr->second.inputChannel.id;
        for (int i=0; i<2; i++)
        {
            int out = itr->second.aimingDirections[i];

            if ( out != Nodrctn )
                _input_arb[in]->AddRequest(out, out, 0);

        }


        // cout << "input_arb" << in_id << "add output"<<resourceId <<endl;

    }

    //input arbitration
    for (int i=0; i<_inputs; i++)
    {
        if (_input_arb[i]->_num_reqs>0){
            int label = -1;
            const int output = _input_arb[i]->Arbitrate(&label, NULL);
            //cout << "output_arb" << output << "add input"<<i << "its position is "<<_input_arb[i]->current_pointer()<<endl;
            assert(output > -1);
            _output_arb[output]->AddRequest(i, i, 0);

        }
    }
    //output arbitration
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
            _input_arb[input]->UpdateState() ;
            _output_arb[i]->UpdateState() ;
            //  cout << "output_arb" << i << "grant input"<<input << "its position is "<<_input_arb[input]->current_pointer()<<endl;
            _output_arb[i]->Clear();
        }

    }
    for (int i=0; i<NUM_DIRECTIONS; i++)
    {
        _input_arb[i]->Clear();
    }

    for (itr = _newTrans.begin(); itr!= _newTrans.end(); itr++)
    {
        int in=itr->second.inputChannel.id;
        if (_in_out_match[in]!= Nodrctn)
        {
            for (int i=0; i<2; i++)
            {
                int out = itr->second.aimingDirections[i];
                if ( (out != Nodrctn )  && (_out_in_match[out]==in)) {
                    // cout << "add " << " in " << in << " out " << out <<endl;
                    Channel c;
                    c.id=_in_out_match[in];
                    itr->second.outputChannel.push_back(c);
                }
            }
        }
    }
    clear();

}

Allocator *Allocator::NewAllocator ( std::string _name,  int inputs, int outputs, inputVec &  newTrans, SlotTable & slotTable,std::string alloc_type)
{
    Allocator *a = 0;

    if ( alloc_type == "input_first" ) {
        a = new Input_first_allocator( _name, inputs, outputs, newTrans, slotTable );
    }
    if ( alloc_type == "waterfall" ) {
        a = new Waterfall( _name, inputs, outputs, newTrans, slotTable );
    }


    //==================================================
    // Insert new allocators here, add another else if
    //==================================================


    return a;

}




