/*
 * SlotTable.cpp
 *
 *  Created on: Mar 1, 2013
 *      Author: lsteng
 */

#include "SlotTable.h"
using namespace std;
using namespace PPB;

SlotTable::SlotTable(int depth, const char* name) {
    // TODO Auto-generated constructor stub
    string k =name;

    _name = "slot_table"+k;
    _depth= depth;
    slot_Table.resize (depth);
    slotStatus.resize(depth);
    slotValid.resize (depth);
    slotNotConfirmed.resize(depth);
for (int i=0; i<depth; i++)
     {
       slot_Table[i].resize(NUM_DIRECTIONS,0);
       slotStatus[i].resize(NUM_DIRECTIONS,Idle);
       slotValid[i].resize(NUM_DIRECTIONS,true);
       slotNotConfirmed[i].resize(NUM_DIRECTIONS,true);
     }


}

string SlotTable::getName() {
    return _name;
}



  vector<int> SlotTable::readSlots (const int &slot ) const {
        return slot_Table [slot];

    }

  vector <bool> SlotTable::checkValids(const int &slot) const {

      return slotValid[slot];
  }

  vector <bool> SlotTable::checkConfirms(const int &slot) const {

       return slotNotConfirmed[slot];
   }

  bool SlotTable::confirmASlot(const int &slot, const int &outValue) {
          if (slotNotConfirmed[slot][outValue] == false )
                  return false;
          else
          {

              slotNotConfirmed[slot][outValue]=false;
              return true;
          }

      }

   bool SlotTable::writeASlot(const int &slot, const int &inValue, const int &outValue) {
        if (slotValid[slot][outValue] == false )
                return false;
        else
        {
            slot_Table[slot][outValue] =inValue;
            slotValid[slot][outValue]=false;
            return true;
        }

    }
    PPB::channel_status_enum SlotTable::checkSlotStatus (const int &slot, const int &outValue) {

        return slotStatus[slot][outValue];

    }

     bool SlotTable::updateSlotStatus (const int &slot, const int & outValue, const PPB::channel_status_enum status) {
        if ( slotStatus[slot][outValue] != Idle && status == Booked  )   // higher level protocols must ensure valid bit is false;
        {
            return false;
        }
        else if ( slotStatus[slot][outValue] != Booked && status == Cancellation )
        {
            return false;
        }
        else
        {
            slotStatus[slot][outValue] = status;
            return true;
        }
    }


   int SlotTable::eraseSlots () {
        for (int i=0; i<_depth; i++)
            for (int j=0; j< NUM_DIRECTIONS;j++)
            {
                if (slotStatus[i][j]==Cancellation)
                {
                    slotStatus[i][j]=Idle;
                    slotValid[i][j]=true;
                    slot_Table[i][j]=0;
                    slotNotConfirmed[i][j]=true;
                   // return 1;
                }

            }
        return 2;

    }
    int SlotTable::read_eraseSlot (const int &slot, const int & outValue)
    {
        markErasedSlot (slot, outValue);
        return slot_Table[slot][outValue];

    }
   bool SlotTable::markErasedSlot (const int &slot, const int &outValue)
    {
        if (slotValid[slot][outValue]== true)
        {
            throw Router_err ("the marked slot is valid", _name);
            return false;

        }
        if (slotStatus[slot][outValue] != Booked)
        {
                   throw Router_err ("the marked slot is not in booked state", _name);
                   return false;

        }
        slotStatus[slot][outValue]=Cancellation;
        return true;

    }

   bool SlotTable::compare(const int &slot, const int &inValue, const int &outValue) {

       if ( slotValid[slot][outValue] == true )
       {
           //cout << "the compared slot is not used " << _name <<endl;
                             return false;
       }

       if (slot_Table[slot][outValue] != inValue)
       {
           //cout << "the compared slot is not set correctly" << _name <<endl;
       return false;
       }

       return true;
   }

SlotTable::~SlotTable() {
    // TODO Auto-generated destructor stub
}

