#ifndef BEHAVIOURAL_PERSISTENTSELECTOR_H_
#define BEHAVIOURAL_PERSISTENTSELECTOR_H_
#include <string>
#include <vector>
#include "../structural/PersistentSelectorSIM.h"

class PersistentSelector: public PersistentSelectorSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(PersistentSelector);
  /*Constructor*/
  explicit PersistentSelector(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~PersistentSelector() = default;

 public:
  void init();
  int pktTxRate;

 private:
  void PersistentSelector_PortServiceThread();
  void PersistentSelectorThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
};

#endif  // BEHAVIOURAL_PERSISTENTSELECTOR_H_
