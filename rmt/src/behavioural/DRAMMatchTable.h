#ifndef BEHAVIOURAL_DRAMMATCHTABLE_H_
#define BEHAVIOURAL_DRAMMATCHTABLE_H_
#include <string>
#include <vector>
#include "../structural/DRAMMatchTableSIM.h"

class DRAMMatchTable: public DRAMMatchTableSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(DRAMMatchTable);
  /*Constructor*/
  explicit DRAMMatchTable(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~DRAMMatchTable() = default;

 public:
  void init();
  int pktTxRate;

 private:
  void DRAMMatchTable_PortServiceThread();
  void DRAMMatchTableThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
};

#endif  // BEHAVIOURAL_DRAMMATCHTABLE_H_