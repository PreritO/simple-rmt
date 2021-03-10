#ifndef BEHAVIOURAL_NVMSEARCHENGINES_H_
#define BEHAVIOURAL_NVMSEARCHENGINES_H_

#include "systemc.h"
#include "tlm.h"
#include "tries/src/RoutingTableEntry.h"

using tlm::tlm_tag;

class NVMActionBase {};

template<class T>
class NVMAction: public NVMActionBase {
 public:
  NVMAction() {}
  explicit NVMAction(T action) : action(action) {}
  NVMAction(const NVMAction& nvm_action) {
    this->action = nvm_action.getAction();
  }

  // Getter
  T getAction() const {
    return action;
  }

  // Operator Overloading
  NVMAction& operator=(const NVMAction& nvm_action) {
    if (&nvm_action != this) {
      this->action = nvm_action.getAction();
    }
    return *this;
  }

  NVMAction& operator=(const T& action) {
    this->action = action;
    return *this;
  }

 private:
  T action;
};

class NVMSearchEngineS : public sc_interface {
 public:
  /* User Logic - Virtual Functions for interface go here */
  // Insert
  virtual void insert(BitString prefix, NVMActionBase* action) = 0;
  virtual void insert(RoutingTableEntry<NVMActionBase*> *routingTable,
        unsigned int routingTableSize) = 0;

  // Remove
  virtual void remove(BitString prefix) = 0;

  // Reconstruct
  virtual void reconstruct(RoutingTableEntry<NVMActionBase*> *routingTable,
        unsigned int routingTableSize) = 0;

  // Search
  virtual NVMActionBase* exactSearch(BitString prefix) = 0;
  virtual NVMActionBase* search(BitString prefix) = 0;

  virtual void setDefaultAction(NVMActionBase* action) = 0;
};

#endif  // BEHAVIOURAL_NVMSEARCHENGINES_H_
