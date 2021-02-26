#ifndef BEHAVIOURAL_DRAMMEMI_H_
#define BEHAVIOURAL_DRAMMEMI_H_

#include "systemc.h"
#include "tlm.h"
#include "DramSearchEngineS.h"

using tlm::tlm_tag;

template <typename T>
class DramMemI : public sc_interface {
 public:
  /* User Logic - Virtual Functions for interface go here */
  virtual void write(DramActionBase* action, unsigned int addr) = 0;
  virtual DramActionBase* read(unsigned int addr) = 0;
};

#endif  // BEHAVIOURAL_DramMEMI_H_
