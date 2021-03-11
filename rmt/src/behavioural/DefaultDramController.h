#ifndef BEHAVIOURAL_DEFAULTDRAMCONTROLLER_H_
#define BEHAVIOURAL_DEFAULTDRAMCONTROLLER_H_
#include <vector>
#include <string>
#include "../structural/DefaultDramControllerSIM.h"

class DefaultDramController: public DefaultDramControllerSIM {
 public:
  SC_HAS_PROCESS(DefaultDramController);
  /*Constructor*/
  explicit DefaultDramController
      (sc_module_name nm, pfp::core::PFPObject* parent = 0,
      std::string configfile = "");
  /*Destructor*/
  virtual ~DefaultDramController() = default;

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

  static bool sortEntries(RoutingTableEntry<DramActionBase*> iFirst,
        RoutingTableEntry<DramActionBase*> iSecond);

  virtual void setDefaultAction(DramActionBase* action);

  //  r/w function to read wrtie to mem
  int addrcounter = 0;
  void write_mem(DramActionBase* data_to_allocate, int addr);
  DramActionBase* read_mem(int addr);

 private:
  void DefaultDramController_PortServiceThread();
  void DefaultDramControllerThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;

  int actionTableSize;
  long long capacity;
  DramActionBase* defaultAction;

  int actionTableSizeBytes;
};
#endif  // BEHAVIOURAL_DEFAULTDramCONTROLLER_H_
