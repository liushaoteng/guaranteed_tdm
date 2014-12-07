/*
 * SlotTable.h
 *
 *  Created on: Mar 1, 2013
 *      Author: lsteng
 */

#ifndef SLOTTABLE_H_
#define SLOTTABLE_H_
#include "networkdef.h"
#include <map>
#include <string>
#include <vector>


class SlotTable {
public:
    SlotTable(int depth, const char * _name);
    virtual ~SlotTable();
private:
    std::vector < std::vector<int> > slot_Table;
    std::vector < std::vector<PPB::channel_status_enum> > slotStatus;
    std::vector < std::vector<bool> > slotValid;
    std::vector <std:: vector <bool>  > slotNotConfirmed;

    //for test purpose, we can build a map with trans and properties for each time slot;

    //std::vector <int> eraseMark;


    std::string _name;
    int _depth;
public:
    std::string getName();
    virtual std::vector <int> readSlots (const int &slot) const;
    virtual std::vector <bool> checkValids (const int &slot) const;
    virtual std::vector <bool> checkConfirms(const int &slot) const;
    bool confirmASlot(const int &slot, const int &outValue);
    virtual bool writeASlot(const int &slot, const int &inValue, const int &outValue);
    virtual PPB::channel_status_enum checkSlotStatus (const int &slot, const int &outValue);
    virtual bool updateSlotStatus (const int &slot, const int & outValue, const PPB::channel_status_enum);
    virtual int eraseSlots ();
    virtual int read_eraseSlot (const int &slot, const int & outValue);
    virtual bool markErasedSlot (const int &slot, const int & outValue);

    bool compare (const int &slot, const int &inValue, const int &outValue);
};

#endif /* SLOTTABLE_H_ */
