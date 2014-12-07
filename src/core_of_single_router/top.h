/*
 * top.h
 * This version is just for a simple test with 1 rsc and 1 router
 *  Created on: Mar 21, 2012
 *      Author: lsteng
 */

#ifndef TOP_H_
#define TOP_H_
#include "commonheader.h"
#include "simpleresource_TDM.h"
#include "TDMRouter.h"
#include "assert.h"
#include "DummyModule.h"

namespace PPB {

class Top: sc_core::sc_module {
public:
    SimpleResource_TDM  * resource[NUM_NODES];
    TDM_Router * router[NUM_SUB_NETWORKS][NUM_NODES];
    DummyModule* dummy[NUM_SUB_NETWORKS];
    sc_core::sc_event updateSlot;

    SC_CTOR(Top) {
      // resource = new SimpleResource * [NUM_SUB_NETWORKS];
      // router = new Router *[NUM_SUB_NETWORKS];
        

        /** initialization componets*/
      for (int col = 0; col < COL; col++)
            for (int row = 0; row < ROW; row++) {
                int self = col + row * COL;
                char txt_rsc[20];
                
                sprintf(txt_rsc, "node_(%d,%d)", row,col);
               

                resource[self] = new SimpleResource_TDM(txt_rsc, row, col);
               
                std::cout <<"generating rsc col "<<col<<" row "<<row<<std::endl;
            }



      for (int k=0; k < NUM_SUB_NETWORKS; k++)
    {     
     
      dummy[k] = new DummyModule("dummy_(0,0)", 0, k);
      
      //  resource [k] = new SimpleResource [NUM_NODES];
     // router [k] = new Router[NUM_NODES];
      

        for (int col = 0; col < COL; col++)
            for (int row = 0; row < ROW; row++) {
                int self = col + row * COL;
               
                char txt_router[20];
              
                sprintf(txt_router, "router_(%d,%d,%d)", row,col,k);

              
                router[k][self] = new TDM_Router(txt_router, row, col);
                std::cout <<"generating router col "<<col<<" row "<<row<<std::endl;
            }
    }

      

 

      for (int k=0; k < NUM_SUB_NETWORKS; k++)
    {

        for (int col = 0; col < COL; col++)
            for (int row = 0; row < ROW; row++) {
                int self = col + row * COL;
                int northNeighbor = col +1+ (row) * COL;
                int eastNeighbor = col + (row+1) * COL;
                int southNeighbor = col -1+ (row) * COL;
                int westNeighbor = col + (row-1) * COL;
                int portId;
                int nbPortId;

                /**resouce binding*/
    
          
                for (int i = 0; i < CHANNEL_PER_DIRECTION; i++) {
                    portId = Resource * CHANNEL_PER_DIRECTION + i;
                    resource[self]->send_socket[i+k*CHANNEL_PER_DIRECTION]->bind(
                            *router[k][self]->in_socket[portId]);
                    router[k][self]->out_socket[portId]->bind(
                            resource[self]->recieve_socket);
                }

                for (int i = 0; i < CHANNEL_PER_DIRECTION; i++) {
                    std::cout<<"binding " << col <<","<<" row " << row <<" i is "<<i<<std::endl;
                    /**North binding*/
                    portId = North * CHANNEL_PER_DIRECTION + i;
                    nbPortId = South * CHANNEL_PER_DIRECTION + i;
                    if (col != COL-1) {

                        router[k][self]->out_socket[portId]->bind(
                                *router[k][northNeighbor]->in_socket[nbPortId]);
                    } else {
                        router[k][self]->out_socket[portId]->bind(
                                dummy[k]->recieve_socket);
                        dummy[k]->send_socket.bind(
                                *router[k][self]->in_socket[portId]);
                    }

                    /**East binding*/
                    portId = East * CHANNEL_PER_DIRECTION + i;
                    nbPortId = West * CHANNEL_PER_DIRECTION + i;
                    if (row != ROW-1) {

                        router[k][self]->out_socket[portId]->bind(
                                *router[k][eastNeighbor]->in_socket[nbPortId]);
                    } else {
                        router[k][self]->out_socket[portId]->bind(
                                dummy[k]->recieve_socket);
                        dummy[k]->send_socket.bind(
                                *router[k][self]->in_socket[portId]);
                    }
                    /**South binding*/
                    portId = South * CHANNEL_PER_DIRECTION + i;
                    nbPortId = North * CHANNEL_PER_DIRECTION + i;
                    if (col != 0) {

                        router[k][self]->out_socket[portId]->bind(
                                *router[k][southNeighbor]->in_socket[nbPortId]);
                    } else {
                        router[k][self]->out_socket[portId]->bind(
                                dummy[k]->recieve_socket);
                        dummy[k]->send_socket.bind(
                                *router[k][self]->in_socket[portId]);
                    }

                    /**West binding*/
                    portId = West * CHANNEL_PER_DIRECTION + i;
                    nbPortId = East * CHANNEL_PER_DIRECTION + i;
                    if (row != 0) {

                        router[k][self]->out_socket[portId]->bind(
                                *router[k][westNeighbor]->in_socket[nbPortId]);
                    } else {
                        router[k][self]->out_socket[portId]->bind(
                                dummy[k]->recieve_socket);
                        dummy[k]->send_socket.bind(
                                *router[k][self]->in_socket[portId]);
                    }
                }
            }
        dummy[k]->send_socket.bind(dummy[k]->recieve_socket);

//        /** west edge binding*/
//        int col = 0;
//        for (int row = 0; row < ROW; row++) {
//            int self = col + row * COL;
//            //resource[self] = new SimpleResource("node (row,col)", row, col);
//            //router[self] = new Router("router (row,col)", row, col);
//            for (int i = 0; i < CHANNEL_PER_DIRECTION; i++) {
//                int portId = West * CHANNEL_PER_DIRECTION + i;
//                router[self]->out_socket[portId]->bind(dummy->recieve_socket);
//            }
//        }
//        /** East edge binding*/
//        int col = COL;
//        for (int row = 0; row < ROW; row++) {
//            int self = col + row * COL;
//            for (int i = 0; i < CHANNEL_PER_DIRECTION; i++) {
//                int portId = East * CHANNEL_PER_DIRECTION + i;
//                router[self]->out_socket[portId]->bind(dummy->recieve_socket);
//            }
//        }
//
//        /** South edge binding*/
//        int row = 0;
//        for (int col = 0; col < COL; col++) {
//            int self = col + row * COL;
//            for (int i = 0; i < CHANNEL_PER_DIRECTION; i++) {
//                int portId = South * CHANNEL_PER_DIRECTION + i;
//                router[self]->out_socket[portId]->bind(dummy->recieve_socket);
//            }
//        }
//
//        /** North edge binding*/
//        int row = ROW;
//        for (int col = 0; col < COL; col++) {
//            int self = col + row * COL;
//            for (int i = 0; i < CHANNEL_PER_DIRECTION; i++) {
//                int portId = North * CHANNEL_PER_DIRECTION + i;
//                router[self]->out_socket[portId]->bind(dummy->recieve_socket);
//            }
//        }
     }


     // SC_METHOD(updateSlotTime);
       //      sensitive << updateSlot;

    }
    void updateSlotTime ();


    virtual ~Top();
};

}
/* namespace PPB */
#endif /* TOP_H_ */
