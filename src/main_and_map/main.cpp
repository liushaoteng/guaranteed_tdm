/*
 * main.cpp
 *
 *  Created on: Jan 16, 2012
 *      Author: lsteng
 */

//#include "../core_of_single_router/Msg.h"
//#include "../core_of_single_router/MemManager.h"
//#include "../core_of_single_router/Routerstorage.h"
#include "../core_of_single_router/TDMRouter.h"
//#include "TDMRouter.h"
#include "../core_of_single_router/simpleresource_TDM.h"
//#include "simpleresource_TDM.h"
//#include "../test_functions/tbRouterstorage.h"
#include "../core_of_single_router/top.h"
//#include "top.h"
#include <iostream>
#include "systemc.h"
using namespace std;
using namespace PPB;
//using namespace Message;
//#ifdef _INPUT_FROM_FILE

//#endif

sc_event bow;
SC_MODULE (hello_world) {
    SC_CTOR (hello_world) {
        SC_METHOD(say_hello);
        sensitive << bow;
        dont_initialize();
        SC_THREAD(call_say_hello);

        // Nothing in constructor
    }

    void say_hello() {
        //Print "Hello World" to the console.
        static int n = 0;
        n++;
        cout << n << " I will be a legend" << " time " << sc_time_stamp()
                << endl;
    }
    void call_say_hello() {
        //bow.notify();
        bow.notify(2, SC_NS);
        bow.notify(1, SC_NS);

    }
};

//class less_test {
//public:
//    bool operator()(ProbeProperty const & _A, ProbeProperty const & _B) const {
//
//        if (_A.highPriority < _B.highPriority)
//            return true;
//        else if (_A.highPriority == _B.highPriority) {
//            if (_A.lowPriority < _B.lowPriority)
//                return true;
//            else
//                return _A.srcId < _B.srcId;
//        }
//        return false;
//    }
//
//    bool operator()(ProbeProperty * _A, ProbeProperty * _B) const {
//
//        if (_A->highPriority < _B->highPriority)
//            return true;
//        else if (_A->highPriority == _B->highPriority) {
//            if (_A->lowPriority < _B->lowPriority)
//                return true;
//            else
//                return _A->srcId < _B->srcId;
//        }
//        return false;
//    }
//};

// sc_main in top level function like in C++ main
int sc_main(int argc, char* argv[]) {
    //hello_world hello1("HELLO1");
    // hello_world hello2("HELLO2");
    //hello1.
    // Print the hello world
    //hello.say_hello();
    // Routerstorage storage;
    /* below are tests*/

//    std::map<ProbeProperty*, int, less_test> test;
//    std::map<ProbeProperty*, int, less_test>::iterator itr;
//    std::vector<int> p;
//
//    int timp = 5;
//    p.push_back(timp);
//
//    ProbeProperty *property1 = new ProbeProperty;
//    ProbeProperty *property2 = new ProbeProperty;
//    ProbeProperty *property3 = new ProbeProperty;
//    // ProbeProperty & ppt= *property1;
//
//    property1->srcId = 55;
//    property1->lowPriority = 3;
//    property1->highPriority = 5;
//
//    property1->dstId = 3;
//
//    property2->srcId = 55;
//    property2->lowPriority = 3;
//    property2->highPriority = 4;
//
//    property2->dstId = 4;
//
//    property3->srcId = 55;
//    property3->lowPriority = 3;
//    property3->highPriority = 4;
//
//    property3->dstId = 5;
//
//    test[property1] = 4;
//    test[property2] = 6;
//    test[property3] = 5;

//    for (itr = test.begin(); itr != test.end(); itr++) {
//        cout << "map interaion's value: " << itr->second << endl;
//    }
//    // cout <<"before delete " <<ppt.lowPriority<<endl;
//    delete property1;
//    // cout <<"after delete " <<ppt.lowPriority<<endl;
//    cout << "after delete " << property1->lowPriority << endl;
    /*
     itr=test[0].find(property1);
     */
    //cout << "map 1 iteraion's value: " << itr->second <<endl;
   // tb_Routerstorage tbStorage;
    //tbStorage.tb_round_robin_allocation();
    /*above are useless tests*/
    if (argc<2)
        {
        cout<<"###########################################################"<<endl;
        cout<<"please input test bench path, run program like :"<<endl;
        cout<<"./ppb_cpp /inp_test_bench/4 200"<<endl;
        cout<<"good luck!"<<endl;
        cout<<"###########################################################"<<endl;
        }

    sprintf (path, "%s", argv[1]);
    lifeTime=atoi(argv[2]);
    cout << path<<" lifetime is " <<lifeTime<<endl;
    if (CHANNEL_PER_DIRECTION ==1 )
	{
	scale_factor=1.0;
	}
    else if (CHANNEL_PER_DIRECTION ==2 )
	{
	scale_factor=1.174;
	}
    else if (CHANNEL_PER_DIRECTION ==4 )
	{
	scale_factor=1.251 ;
	}
    else
	{
	scale_factor=1.0;
	}
 cout << " scale_factor is "<< scale_factor <<endl;
#ifdef  _INPUT_FROM_FILE
    char temp[256];

    while (!cin.eof()) {
        cin >> temp;
        if (temp[0] == '#')
        {   char ignore[256];
            cin.getline(ignore,256);
        }
        else if (temp[0] == '.') {
            if (strcmp(temp, ".COL"))
            cin >> COL;
            else if (strcmp(temp, ".ROW"))
            cin >> ROW;
            else
            cout << "no such input variable" << temp << endl;
        }

        else
        cout << "no such input format" << temp << endl;

    }
    cout<<"$$$$begin of parameter configuration$$$$$$$$$$$$$$$$$"<<endl;
    cout<<" COL "<<COL<<endl;
    cout<<" ROW "<<ROW<<endl;
    cout<<" COL_BITS "<<COL_BITS<<endl;
    cout<<" NUM_NODES "<<NUM_NODES<<endl;
    cout<<" NUM_DIRECTIONS "<<NUM_DIRECTIONS<<endl;
    cout<<" CHANNEL_FANOUTS "<<CHANNEL_FANOUTS<<endl;
    cout<<" TOTAL_NUM_CHANNELS "<<TOTAL_NUM_CHANNELS<<endl;
    cout<<" RESOURCE_CHANNEL_NUM "<<RESOURCE_CHANNEL_NUM<<endl;
    cout<<" CHANNEL_PER_DIRECTION "<<CHANNEL_PER_DIRECTION<<endl;

#endif
   //commented for test purposes

     Top top("top");
     try {
    sc_set_time_resolution(1,SC_NS);
    //sc_start();
    sc_start(MAX_TIME,SC_NS);

     } catch (const Router_err &e)
     {
         cerr<<e.what() << e.name()<<endl;
     }
     catch (const exception &e){
         cerr<< "unknown error" <<endl;
     }
    //commented for test purposes
    cout << "simulation is finished" << endl;
    //Msg<3> p;
    sc_uint<3> t;
    //Message::Msg<3> *p=new Message::Msg <3>;
    //p=1;
    //cout << t << endl;
    //p=0;
    //p[1]=1;
//    t = 0;
//    t[1] = 1;
    //cout << t;
    return (0);
}

