/*
 * Msg.h
 *
 *  Created on: Jan 16, 2012
 *      Author: lsteng
 */

#ifndef MSG_H_
#define MSG_H_
#include "systemc.h"
//#define n 2

namespace Message {

template <int n>
class Msg :sc_uint<n>{
private:
	//int n;
	//sc_uint <n> value;
public:


	Msg(void);
	virtual ~Msg();
	using sc_uint<n>::operator =;
	using sc_uint<n>::operator [];
	//sc_uint<n> getValue();
	//void putValue(sc_uint<n> t);


};

template <int n>
Msg<n>::Msg(void) {
	// TODO Auto-generated constructor stub
	//value =0;

}

template <int n>
Msg<n>::~Msg() {
	// TODO Auto-generated destructor stub
}
/*
template <int n>
void Msg<n>::putValue(sc_uint<n> t){
	value=t;
}

template <int n>
sc_uint<n> Msg<n>::getValue(){
	return value;
}
*/
} /* namespace Msg */

#endif /* MSG_H_ */

