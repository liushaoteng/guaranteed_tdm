/*
 * allocator.h
 *
 *  Created on: Mar 3, 2013
 *      Author: lsteng
 */

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_
#include "commonheader.h"
#include "networkdef.h"

#include "assert.h"
#include "peq_with_get_map.h"
#include "Arbitor.h"
#include "SlotTable.h"
#include <ostream>


class Allocator {
public:
    typedef std::map<tlm::tlm_generic_payload*, PPB::ProbeProperty>  inputVec;
    Allocator(std::string name,  int inputs, int outputs, inputVec &  newTrans, SlotTable & slotTable):_inputs (inputs),_outputs(outputs),_newTrans(newTrans),_slotTable(slotTable),_name(name) {
        //_newTrans=newTrans;
        //_slotTable=slotTable;
        _in_out_match.resize(_inputs);
        _out_in_match.resize(_outputs);
        clear();


    }

    //virtual void AddRequest( std::map<tlm::tlm_generic_payload*, ProbeProperty> & newTrans)=0;
    // virtual void RemoveRequest( int in, int out, int label = 1 ) = 0;

     virtual void Allocate( int & currentPosSLot) =0;


     virtual void clear( );

    // int OutputAssigned( int in ) const;
     //  int InputAssigned( int out ) const;
       std::string get_name () const { return _name; }

       static Allocator *NewAllocator( std::string _name,  int inputs, int outputs, inputVec &  newTrans, SlotTable & slotTable, std::string alloc_type);

    virtual ~Allocator();

protected:

       bool filter(int & currentPosSLot);



       std::vector<int> _in_out_match;
       std::vector <int>  _out_in_match;
       const int _inputs;
       const int _outputs;
       inputVec &  _newTrans;
       SlotTable & _slotTable;

      // std::multimap< int,  PPB::ProbeProperty &> result_map;
private :
       std::string _name;




};



class Sparse_allocator : public Allocator{
public:
    Sparse_allocator (std::string name,  int inputs, int outputs, Allocator::inputVec &  newTrans, SlotTable & slotTable) :
        Allocator (name, inputs, outputs, newTrans, slotTable) {
        std::string arb_type= "round_robin";
           _input_arb.resize(_inputs);
           for (int i = 0; i < _inputs; ++i) {
             std::ostringstream arb_name("arb_i");
             arb_name << i;


             _input_arb[i] = Arbiter::NewArbiter( arb_name.str(), arb_type, _outputs);    // this place can be optimized by reduce _outputs to two;
           }

           _output_arb.resize(_outputs);

           for (int i = 0; i < _outputs; ++i) {
             std::ostringstream arb_name("arb_o");
             arb_name << i;
             _output_arb[i] = Arbiter::NewArbiter(arb_name.str( ), arb_type, _inputs);
           }

    }

    virtual void clear () {
        for ( int i = 0 ; i < _inputs ; i++ ) {
           if(_input_arb[i]-> _num_reqs)
             _input_arb[i]->Clear();
         }
         for ( int o = 0; o < _outputs; o++ ) {
           if(_output_arb[o]->_num_reqs)
             _output_arb[o]->Clear();
         }
         Allocator::clear();

    }
    virtual ~Sparse_allocator () {
        for (int i = 0; i < _inputs; ++i) {
        delete _input_arb[i];
      }

      for (int i = 0; i < _outputs; ++i) {
        delete _output_arb[i];
      }
    }


protected:
    std::vector<Arbiter*> _input_arb;
    std::vector<Arbiter*> _output_arb ;
};



class Input_first_allocator: public Sparse_allocator {
public:
    Input_first_allocator (std::string _name,  int inputs, int outputs, inputVec &  newTrans, SlotTable & slotTable) :
           Sparse_allocator (_name, inputs, outputs, newTrans, slotTable) {

    }
    virtual void Allocate( int & currentPosSLot);

};




#endif /* ALLOCATOR_H_ */
