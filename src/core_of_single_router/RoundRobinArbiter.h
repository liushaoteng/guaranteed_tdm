/*
 * Rrariber.h
 *
 *  Created on: Oct 23, 2012
 *      Author: lsteng
 */

#ifndef RRARIBER_H_
#define RRARIBER_H_

#include "Arbitor.h"

class RoundRobinArbiter: public Arbiter {
public:
    int  _pointer ;

    public:

      // Constructors
      RoundRobinArbiter(const std::string &name, int size );

      // Print priority matrix to standard output

      virtual void UpdateState() ;
      virtual void rotateState(int );
      virtual int current_pointer() {
          return _pointer;
      }

      // Arbitrate amongst requests. Returns winning input and
      // updates pointers to metadata when valid pointers are passed
      virtual int Arbitrate( int* id = 0, int* pri = 0);

      virtual void AddRequest( int input, int id, int pri ) ;

      virtual void Clear();
      static inline bool Supersedes(int input1, int pri1, int input2, int pri2, int offset, int size)
        {
          // in a round-robin scheme with the given number of positions and current
          // offset, should a request at input1 with priority pri1 supersede a
          // request at input2 with priority pri2?
          return ((pri1 > pri2) ||
              ((pri1 == pri2) &&
               (((input1 - offset + size) % size) < ((input2 - offset + size) % size))));
        }
    virtual ~RoundRobinArbiter(){ };

};

#endif /* RRARIBER_H_ */
