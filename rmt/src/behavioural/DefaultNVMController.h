#ifndef BEHAVIOURAL_DEFAULTNVMCONTROLLER_H_
#define BEHAVIOURAL_DEFAULTNVMCONTROLLER_H_
#include <vector>
#include <string>
#include "../structural/DefaultNVMControllerSIM.h"

class DefaultNVMController: public DefaultNVMControllerSIM {
 public:
  SC_HAS_PROCESS(DefaultNVMController);
  /*Constructor*/
  explicit DefaultNVMController
      (sc_module_name nm, pfp::core::PFPObject* parent = 0,
      std::string configfile = "");
  /*Destructor*/
  virtual ~DefaultNVMController() = default;

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

  static bool sortEntries(RoutingTableEntry<NVMActionBase*> iFirst,
        RoutingTableEntry<NVMActionBase*> iSecond);

  virtual void setDefaultAction(NVMActionBase* action);

  //  r/w function to read wrtie to mem
  int addrcounter = 0;
  void write_mem(NVMActionBase* data_to_allocate, int addr);
  NVMActionBase* read_mem(int addr);

 private:
  void DefaultNVMController_PortServiceThread();
  void DefaultNVMControllerThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;

  int actionTableSize;
  int capacity;
  NVMActionBase* defaultAction;
};
#endif  // BEHAVIOURAL_DEFAULTNVMCONTROLLER_H_
