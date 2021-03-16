#include "./PersistentPipeline.h"
#include <string>

PersistentPipeline::PersistentPipeline(sc_module_name nm, pfp::core::PFPObject* parent,std::string configfile ):PersistentPipelineSIM(nm,parent,configfile) {  // NOLINT(whitespace/line_length)
  /*sc_spawn threads*/
}

void PersistentPipeline::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}
void PersistentPipeline::PersistentPipeline_PortServiceThread() {
  // Thread function to service input ports.
}
void PersistentPipeline::PersistentPipelineThread(std::size_t thread_id) {
  // Thread function for module functionalty
}

void PersistentPipeline::configure() {
  for (int i = 0; i < 32; i++) { // 32 stages
    match_stage[i]->configure();
  }
  // assume the nat table for this, TODO: modify this to be dynamic
  
  // for (auto &p : parser) {
  //   p->set_first_table(config_list[0].name);
  // }
}
