#ifndef BEHAVIOURAL_NVMMEMI_H_
#define BEHAVIOURAL_NVMMEMI_H_

#include "systemc.h"
#include "tlm.h"
#include "NVMSearchEngineS.h"

using tlm::tlm_tag;

template <typename T>
class NVMMemI : public sc_interface {
 public:
  /* User Logic - Virtual Functions for interface go here */
  virtual void write(NVMActionBase* action, unsigned int addr) = 0;
  virtual NVMActionBase* read(unsigned int addr) = 0;
};

#endif  // BEHAVIOURAL_NVMMEMI_H_
