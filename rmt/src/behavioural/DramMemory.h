#ifndef BEHAVIOURAL_DRAMMEMORY_H_
#define BEHAVIOURAL_DRAMMEMORY_H_
#include <string>
#include <vector>
#include "../structural/DramMemorySIM.h"
#include "DramMemI.h"

const std::string module_name = "DramMemory";

// #define debug_tlm_mem_transaction 1
template<typename T>
class DramMemory:
public DramMemI<T>,
public DramMemorySIM {
 public:
  /* CE Consturctor */
  explicit DramMemory(sc_module_name nm, pfp::core::PFPObject* parent = 0,
        std::string configfile = "");
  /* User Implementation of the Virtual Functions in the Interface.h */

  /* User Implementation of the Virtual Functions in the MemI.h file */

  virtual void write(DramActionBase* action, unsigned int addr) {
    if (addr < SIZE) {
      wait(WR_LATENCY); //influences overall throughput by causing a backlog of packets if the packet generator rate is << than Latency
      //npulog(profile, std::cout << "Writing to MEM @ addr " << std::endl;)
      mem[addr] = action;
    } else {
      SC_REPORT_ERROR("Dram Memory Write", "Out of address range!");
    }
  }

  virtual DramActionBase* read(unsigned int addr) {
    if (addr < SIZE) {
      wait(RD_LATENCY); // this wait matters for lookups - on critical path 
      return mem[addr];
    } else {
      SC_REPORT_ERROR("Dram Memory Read", "Out of address range!");
     }
  }

 private:
  std::string modulename;
  DramActionBase** mem;
  uint64_t SIZE;
  sc_time RD_LATENCY;
  sc_time WR_LATENCY;
};

/*
	DramMemory Consturctor
 */
template<typename T>
DramMemory<T>::DramMemory(sc_module_name nm, pfp::core::PFPObject* parent,
      std::string configfile):DramMemorySIM(nm, parent, configfile) {
  
  int rdlt = GetParameter("ReadLatency").template get<int>();
  int wrlt = GetParameter("WriteLatency").template get<int>();

  int mem_size = GetParameter("Capacity").template get<int>();

  RD_LATENCY = sc_time(rdlt, SC_NS);
  WR_LATENCY = sc_time(wrlt, SC_NS);
  SIZE = mem_size;
  mem = new DramActionBase*[SIZE];  // allocate the block on host mem in heap
}

#endif  // BEHAVIOURAL_DRAMMEMORY_H_
