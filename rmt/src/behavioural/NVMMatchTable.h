#ifndef BEHAVIOURAL_NVMMATCHTABLE_H_
#define BEHAVIOURAL_NVMMATCHTABLE_H_
#include <string>
#include <vector>
#include "../structural/NVMMatchTableSIM.h"
#include "RMTMessages.h"
#include "PacketHeaderVector.h"
#include <map>
#include <fstream>

class NVMMatchTable: public NVMMatchTableSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(NVMMatchTable);
  /*Constructor*/
  explicit NVMMatchTable(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~NVMMatchTable() = default;

 public:
  void init();
  int pktTxRate;

 private:
  void NVMMatchTable_PortServiceThread();
  void NVMMatchTableOriginalThread(std::size_t thread_id);
  void NVMMatchAsyncLookupThread(std::size_t thread_id);
  void NVMMatchAsyncResponseThread(std::size_t thread_id);

  std::vector<sc_process_handle> ThreadHandles;

  MTQueue<std::shared_ptr<PacketHeaderVector>> async_queue;
  sc_event async_rx;

  std::string global_table;

  MTQueue<std::shared_ptr<PacketHeaderVector>> async_response_queue;
  sc_event async_response;
  std::ofstream outlog;
};

#endif  // BEHAVIOURAL_NVMMATCHTABLE_H_
