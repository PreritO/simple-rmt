#ifndef BEHAVIOURAL_DRAMMATCHTABLE_H_
#define BEHAVIOURAL_DRAMMATCHTABLE_H_
#include <string>
#include <vector>
#include "../structural/DRAMMatchTableSIM.h"
#include "RMTMessages.h"
#include "PacketHeaderVector.h"
#include <map>
#include <fstream>



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
  void DRAMMatchTableOriginalThread(std::size_t thread_id);
  void DRAMMatchAsyncLookupThread(std::size_t thread_id);
  void DRAMMatchAsyncResponseThread(std::size_t thread_id);

  std::vector<sc_process_handle> ThreadHandles;

  MTQueue<std::shared_ptr<PacketHeaderVector>> async_queue;
  sc_event async_rx;

  std::string global_table;

  MTQueue<std::shared_ptr<PacketHeaderVector>> async_response_queue;
  sc_event async_response;
  std::ofstream outlog;
};

#endif  // BEHAVIOURAL_DRAMMATCHTABLE_H_
