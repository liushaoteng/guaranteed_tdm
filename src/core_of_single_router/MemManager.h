#ifndef MEMMANAGER_H_
#define MEMMANAGER_H_



#include "commonheader.h"
#include <queue>



#define DEBUG
static  std::ofstream fout("foo.txt");
class gp_mm: public tlm::tlm_mm_interface
{
private:
  typedef tlm::tlm_generic_payload gp_t;

public:
  gp_mm() : free_list(0), empties(0) {}

  virtual ~gp_mm()
  {
    gp_t* ptr;

    while (free_list)
    {
      ptr = free_list->trans;

      // Delete generic payload and all extensions
      assert(ptr);
      delete ptr;

      free_list = free_list->next;
    }

    while (empties)
    {
      access* x = empties;
      empties = empties->next;

      // Delete free list access struct
      delete x;
    }
  }

  gp_t* allocate();
  void free(gp_t* trans);

private:
  struct access
  {
    gp_t* trans;
    access* next;
    access* prev;
  };

  access* free_list;
  access* empties;
};


#endif /* MEMMANAGER_H_ */
