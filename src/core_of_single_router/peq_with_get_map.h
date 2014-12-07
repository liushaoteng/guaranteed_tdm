/*
 * peq_with_get_map.h
 *
 *  Created on: Apr 9, 2012
 *      Author: lsteng
 */

#ifndef PEQ_WITH_GET_MAP_H_
#define PEQ_WITH_GET_MAP_H_

//#include "tlm.h"
#include <systemc>
#include <map>

namespace PPB {

template <class PAYLOAD>
class peq_with_get : public sc_core::sc_object
{
public:
  typedef PAYLOAD transaction_type;
  typedef std::pair<const sc_core::sc_time, transaction_type> pair_type;

public:
  peq_with_get(const char* name) : sc_core::sc_object(name)
  {
  }

  void notify(transaction_type& trans, const sc_core::sc_time& t)
  {
    m_scheduled_events.insert(pair_type(t + sc_core::sc_time_stamp(), trans));
    m_event.notify(t);
  }

  void notify(transaction_type& trans)
  {
    m_scheduled_events.insert(pair_type(sc_core::sc_time_stamp(), trans));
    m_event.notify(); // immediate notification
  }

  // needs to be called until it returns 0
  transaction_type* get_next_transaction()
  {
    if (m_scheduled_events.empty()) {
      return 0;
    }

    sc_core::sc_time now = sc_core::sc_time_stamp();
    if (m_scheduled_events.begin()->first <= now) {
      transaction_type* trans = &( m_scheduled_events.begin()->second);
      m_scheduled_events.erase(m_scheduled_events.begin());
      return trans;
    }

    m_event.notify(m_scheduled_events.begin()->first - now);

    return 0;
  }

  sc_core::sc_event& get_event()
  {
    return m_event;
  }

  // Cancel all events from the event queue
  void cancel_all() {
    m_scheduled_events.clear();
    m_event.cancel();
  }

private:
  std::multimap<const sc_core::sc_time, transaction_type> m_scheduled_events;
  sc_core::sc_event m_event;
};

}




#endif /* PEQ_WITH_GET_MAP_H_ */
