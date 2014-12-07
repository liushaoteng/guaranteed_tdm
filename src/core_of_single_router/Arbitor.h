/*
 * Arbitor.h
 *
 *  Created on: Oct 23, 2012
 *      Author: lsteng
 */

#ifndef ARBITOR_H_
#define ARBITOR_H_
#include "commonheader.h"
#include "networkdef.h"
#include <map>
#include <string>
#include <vector>

class Arbiter {
public:
    typedef struct {
      bool valid ;
      int id ;
      int pri ;
    } entry_t ;

   std::vector<entry_t> _request ;

    Arbiter(const std::string &name,int size);
    virtual ~Arbiter();
    virtual void AddRequest( int input, int id, int pri) ;
    virtual int Arbitrate( int* id = 0, int* pri = 0 ) ;
    static Arbiter * NewArbiter( const std::string &name, const std::string &arb_type, int size);
    virtual void Clear();
    virtual void UpdateState() = 0 ;
    virtual void rotateState(int )=0;
    virtual int current_pointer()=0;
      int  _size ;

      int  _selected ;
      int _highest_pri;
      int _best_input;
      std::string _name;
      int _num_reqs;

};

#endif /* ARBITOR_H_ */
