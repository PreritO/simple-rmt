#ifndef BEHAVIOURAL_DRAMSEARCHENGINE_H_
#define BEHAVIOURAL_DRAMSEARCHENGINE_H_
#include <vector>
#include <string>
#include "structural/DramSearchEngineSIM.h"

class DramSearchEngine: public DramSearchEngineSIM {
 public:
  SC_HAS_PROCESS(DramSearchEngine);
  /*Constructor*/
  explicit DramSearchEngine(sc_module_name nm,
          pfp::core::PFPObject* parent = 0,
          std::string configfile = "");
  /*Destructor*/
  virtual ~DramSearchEngine() = default;

 public:
  void init();

  // Insert
  virtual void insert(BitString prefix, DramActionBase* action);
  virtual void insert(RoutingTableEntry<DramActionBase*> *routingTable,
        unsigned int routingTableSize);

  // Remove
  virtual void remove(BitString prefix);

  // Reconstruct
  virtual void reconstruct(RoutingTableEntry<DramActionBase*> *routingTable,
        unsigned int routingTableSize);

  // Search
  virtual DramActionBase* exactSearch(BitString prefix);
  virtual DramActionBase* search(BitString prefix);

  virtual void setDefaultAction(DramActionBase* action);

 private:
  void DramSearchEngine_PortServiceThread();
  void DramSearchEngineThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
};

#endif  // BEHAVIOURAL_DRAMSEARCHENGINE_H_
