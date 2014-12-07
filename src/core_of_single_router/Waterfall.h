/*
 * waterfall.h
 *
 *  Created on: Jun 20, 2012
 *      Author: lsteng
 */

#ifndef WATERFALL_H_
#define WATERFALL_H_

#include "Allocator.h"
#include <set>

//extern int vc_num;

class Waterfall: public Allocator {
public:
    Waterfall (std::string name,  int inputs, int outputs, Allocator::inputVec &  newTrans, SlotTable & slotTable) :
        Allocator (name, inputs, outputs, newTrans, slotTable)
    {
        //    _availoutput.resize(outputs, 0);
        _current_position.resize(_outputs, 0);
        //    _avail_drctn.resize(_outputs, 0);
        //    _num_requests_for_one_drctn.resize(outputs, 0);
        outputs_sets.resize(_outputs);
        _output_arb.resize(_outputs);
        std::string arb_type= "round_robin";
        for (int i = 0; i < _outputs; ++i) {
            std::ostringstream arb_name("arb_o");
            arb_name << i;
            _output_arb[i] = Arbiter::NewArbiter(arb_name.str( ), arb_type, _inputs);
        }

    }

    virtual ~Waterfall()
    {

        for (int i = 0; i < _outputs; ++i) {
            delete _output_arb[i];
        }

    }
    //virtual void AddRequest ( int in, int out, int label=1, int in_pri=0, int out_pri=0);
    virtual void Allocate( int & currentPosSLot);
protected:
    // std::vector <int> _num_requests_for_one_drctn;
    std::vector <std::set<int> > outputs_sets;
    //  bool getAFreeChannel(int outputdrn, int &output);
private:
    // std::vector <int >_availoutput;
    //   int _num_ports;               //this is number of outputs ports
    std::vector <int> _current_position;

    std::vector<Arbiter*> _output_arb ;
    // std::vector <int> _avail_drctn;
    // int _gNumVCs;

};

#endif /* WATERFALL_H_ */
