#ifndef BEHAVIOURAL_NVMSEARCHENGINE_H_
#define BEHAVIOURAL_NVMSEARCHENGINE_H_
#include <vector>
#include <string>
#include "structural/NVMSearchEngineSIM.h"

class NVMSearchEngine: public NVMSearchEngineSIM {
 public:
  SC_HAS_PROCESS(NVMSearchEngine);
  /*Constructor*/
  explicit NVMSearchEngine(sc_module_name nm,
          pfp::core::PFPObject* parent = 0,
          std::string configfile = "");
  /*Destructor*/
  virtual ~NVMSearchEngine() = default;

 public:
  void init();

  // Insert
  virtual void insert(BitString prefix, NVMActionBase* action);
  virtual void insert(RoutingTableEntry<NVMActionBase*> *routingTable,
        unsigned int routingTableSize);

  // Remove
  virtual void remove(BitString prefix);

  // Reconstruct
  virtual void reconstruct(RoutingTableEntry<NVMActionBase*> *routingTable,
        unsigned int routingTableSize);

  // Search
  virtual NVMActionBase* exactSearch(BitString prefix);
  virtual NVMActionBase* search(BitString prefix);

  virtual void setDefaultAction(NVMActionBase* action);

 private:
  void NVMSearchEngine_PortServiceThread();
  void NVMSearchEngineThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
};

#endif  // BEHAVIOURAL_NVMSEARCHENGINE_H_
