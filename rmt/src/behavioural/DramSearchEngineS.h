#ifndef BEHAVIOURAL_DRAMSEARCHENGINES_H_
#define BEHAVIOURAL_DRAMSEARCHENGINES_H_

#include "systemc.h"
#include "tlm.h"
#include "tries/src/RoutingTableEntry.h"

using tlm::tlm_tag;

class DramActionBase {};

template<class T>
class DramAction: public DramActionBase {
 public:
  DramAction() {}
  explicit DramAction(T action) : action(action) {}
  DramAction(const DramAction& dram_action) {
    this->action = dram_action.getAction();
  }

  // Getter
  T getAction() const {
    return action;
  }

  // Operator Overloading
  DramAction& operator=(const DramAction& dram_action) {
    if (&dram_action != this) {
      this->action = dram_action.getAction();
    }
    return *this;
  }

  DramAction& operator=(const T& action) {
    this->action = action;
    return *this;
  }

 private:
  T action;
};

class DramSearchEngineS : public sc_interface {
 public:
  /* User Logic - Virtual Functions for interface go here */
  // Insert
  virtual void insert(BitString prefix, DramActionBase* action) = 0;
  virtual void insert(RoutingTableEntry<DramActionBase*> *routingTable,
        unsigned int routingTableSize) = 0;

  // Remove
  virtual void remove(BitString prefix) = 0;

  // Reconstruct
  virtual void reconstruct(RoutingTableEntry<DramActionBase*> *routingTable,
        unsigned int routingTableSize) = 0;

  // Search
  virtual DramActionBase* exactSearch(BitString prefix) = 0;
  virtual DramActionBase* search(BitString prefix) = 0;

  virtual void setDefaultAction(DramActionBase* action) = 0;
};

#endif  // BEHAVIOURAL_DRAMSEARCHENGINES_H_
