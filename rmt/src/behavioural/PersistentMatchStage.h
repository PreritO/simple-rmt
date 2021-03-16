#ifndef BEHAVIOURAL_PERSISTENTMATCHSTAGE_H_
#define BEHAVIOURAL_PERSISTENTMATCHSTAGE_H_
#include <string>
#include <vector>
#include "../structural/PersistentMatchStageSIM.h"
#include "PersistentMatchStageConfig.h"

class PersistentMatchStage: public PersistentMatchStageSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(PersistentMatchStage);
  /*Constructor*/
  explicit PersistentMatchStage(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~PersistentMatchStage() = default;

 public:
  void init();

  void configure();

  //! indicates whether the stage as a configuration.
  //! If not, it is an empty stages
  bool has_config = false;


 private:
  void PersistentMatchStage_PortServiceThread();
  void PersistentMatchStageThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
};

#endif  // BEHAVIOURAL_PERSISTENTMATCHSTAGE_H_
