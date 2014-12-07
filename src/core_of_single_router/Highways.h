/*
 * Highways.h
 *
 *  Created on: Jun 13, 2014
 *      Author: waterwood
 */

#ifndef HIGHWAYS_H_
#define HIGHWAYS_H_

#include "commonheader.h"
#include "networkdef.h"

#include "assert.h"
#include "peq_with_get_map.h"

#include "Pseudo_queue.h"
#include "Arbitor.h"

#include <map>
using namespace PPB;

class Highways {
public:
   // typedef std::map<tlm::tlm_generic_payload*, PPB::ProbeProperty>  inputVec;
    Highways(int numOfQueues, int depth, int numEachInDirect, int numEachOutDirect, std::string name );  // currently, no one use the last two parameters
    Highways(int numOfQueues, int depth, std::string name, int numDirections=1);
    virtual ~Highways() {

                for (int i = 0; i < _numDirections; ++i) {
                delete _input_arb[i];
              }

              for (int i = 0; i < _numDirections; ++i) {
                delete _output_arb[i];
              }

    }

    //virtual void addRequest(tlm::tlm_generic_payload*, int input)=0;
//    void addRequest(tlm::tlm_generic_payload*);
//
//    void allocate();
    bool checkHit (tlm::tlm_generic_payload *);

    void configure (tlm::tlm_generic_payload *, int out);

    bool allocate(tlm::tlm_generic_payload*, int in, int slot=-1);
    void presetQueue (tlm::tlm_generic_payload*, int numFlits);
    void markRelease (tlm::tlm_generic_payload*);
    void checkReleaseMark (tlm::tlm_generic_payload*);


    Pseudo_queue* getTheQueue(tlm::tlm_generic_payload*);

    void receiveAFlit (tlm::tlm_generic_payload*);
    bool sendAFlit (tlm::tlm_generic_payload*);   // true if it is normal flit, false if it is tail flit;
    bool readyToSend (tlm::tlm_generic_payload*);

    void updateNextToken (tlm::tlm_generic_payload*, int num);
    void IncreNextToken (tlm::tlm_generic_payload*);
    void DecNextToken (tlm::tlm_generic_payload*);
    void getToken(tlm::tlm_generic_payload* );

    void release (tlm::tlm_generic_payload *);
    void release ();

    std::vector< tlm::tlm_generic_payload* > weighted_arbitrate(std::vector <bool>);

    bool isEmpty(tlm::tlm_generic_payload *);
    bool isFull(tlm::tlm_generic_payload *);
    int checkToken(tlm::tlm_generic_payload *);

    bool isAvailable (int in);
    bool allIsEmpty ();

private:
    const int _numOfQueues;
    const int _depth;
    const int _numEachInDirect;
    const int _numEachOutDirect;
    int _numDirections ;

    int getFreeNumber (int out);

    //virtual int getFreeNumber (int in, int out)=0;

    int nextRoundPointer;
    int nextRoundWinner;



    std::vector < int > numOfFreeQueue;
    std::vector <tlm::tlm_generic_payload*> Request_queue;
    std::map <tlm::tlm_generic_payload*, int> Queue_index;
    std::vector <Pseudo_queue> Queue_repository;
protected:
    std::vector<Arbiter*> _input_arb;
    std::vector<Arbiter*> _output_arb ;
    std:: string _name;
};

#endif /* HIGHWAYS_H_ */
