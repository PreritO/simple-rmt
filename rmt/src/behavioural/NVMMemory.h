#ifndef BEHAVIOURAL_NVMMEMORY_H_
#define BEHAVIOURAL_NVMMEMORY_H_
#include <string>
#include <vector>
#include "../structural/NVMMemorySIM.h"
#include "NVMMemI.h"

//const std::string module_name = "NVMMemory";

// #define debug_tlm_mem_transaction 1
template<typename T>
class NVMMemory:
public NVMMemI<T>,
public NVMMemorySIM {
 public:
  /* CE Consturctor */
  explicit NVMMemory(sc_module_name nm, pfp::core::PFPObject* parent = 0,
        std::string configfile = "");
  /* User Implementation of the Virtual Functions in the Interface.h */

  /* User Implementation of the Virtual Functions in the MemI.h file */

  virtual void write(NVMActionBase* action, unsigned int addr) {
    if (addr < SIZE) {
      wait(WR_LATENCY); //influences overall throughput by causing a backlog of packets if the packet generator rate is << than Latency
      //npulog(profile, std::cout << "Writing to MEM @ addr " << std::endl;)
      mem[addr] = action;
    } else {
      SC_REPORT_ERROR("NVM Memory Write", "Out of address range!");
    }
  }

  virtual NVMActionBase* read(unsigned int addr) {
    if (addr < SIZE) {
      wait(RD_LATENCY); // this wait matters for lookups - on critical path 
      return mem[addr];
    } else {
      SC_REPORT_ERROR("NVM Memory Read", "Out of address range!");
     }
  }

 private:
  std::string modulename;
  NVMActionBase** mem;
  uint64_t SIZE;
  sc_time RD_LATENCY;
  sc_time WR_LATENCY;
};

/*
	NVMMemory Consturctor
 */
template<typename T>
NVMMemory<T>::NVMMemory(sc_module_name nm, pfp::core::PFPObject* parent,
      std::string configfile):NVMMemorySIM(nm, parent, configfile) {
  
  int rdlt = GetParameter("ReadLatency").template get<int>();
  int wrlt = GetParameter("WriteLatency").template get<int>();

  int mem_size = GetParameter("Capacity").template get<int>();

  RD_LATENCY = sc_time(rdlt, SC_NS);
  WR_LATENCY = sc_time(wrlt, SC_NS);
  SIZE = mem_size;
  mem = new NVMActionBase*[SIZE];  // allocate the block on host mem in heap
}

#endif  // BEHAVIOURAL_NVMMEMORY_H_
