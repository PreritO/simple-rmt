#ifndef BEHAVIOURAL_PERSISTENTPIPELINE_H_
#define BEHAVIOURAL_PERSISTENTPIPELINE_H_
#include <string>
#include <vector>
#include "../structural/PersistentPipelineSIM.h"
#include "PersistentMatchStageConfig.h"

class PersistentPipeline: public PersistentPipelineSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(PersistentPipeline);
  /*Constructor*/
  explicit PersistentPipeline(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~PersistentPipeline() = default;

 public:
  void init();
  void configure();


 private:
  void PersistentPipeline_PortServiceThread();
  void PersistentPipelineThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
};

#endif  // BEHAVIOURAL_PERSISTENTPIPELINE_H_
