/*
 * Arbitor.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: lsteng
 */

#include "Arbitor.h"
using namespace std;
#include <limits>
#include <cassert>
#include "RoundRobinArbiter.h"

Arbiter::Arbiter(const string &name,int size) :_size(size), _selected(-1), _highest_pri(numeric_limits<int>::min()),
        _best_input(-1), _name(name),_num_reqs(0){
    // TODO Auto-generated constructor stub
    _request.resize(size);
      for ( int i = 0 ; i < size ; i++ )
        _request[i].valid = false ;
}

Arbiter::~Arbiter() {

    // TODO Auto-generated destructor stub
}







void Arbiter::AddRequest( int input, int id, int pri )
{
  assert( 0 <= input && input < _size ) ;
  assert( !_request[input].valid );

  _num_reqs++ ;
  _request[input].valid = true ;
  _request[input].id = id ;
  _request[input].pri = pri ;
}

int Arbiter::Arbitrate( int* id, int* pri )
{
  if ( _selected != -1 ) {
    if ( id )
      *id  = _request[_selected].id ;
    if ( pri )
      *pri = _request[_selected].pri ;
  }

  assert((_selected >= 0) || (_num_reqs == 0));

  return _selected ;
}

void Arbiter::Clear()
{
  if(_num_reqs > 0) {

    // clear the request vector
    for ( int i = 0; i < _size ; i++ )
      _request[i].valid = false ;
    _num_reqs = 0 ;
    _selected = -1;
  }
}

Arbiter *Arbiter::NewArbiter( const string &name, const string &arb_type, int size)
{
  Arbiter *a = NULL;
  if(arb_type == "round_robin") {
    a = new RoundRobinArbiter(name, size );
  }
  else
  {
      assert (0);
  }
  return a;
}
