#include "./PersistentMatchStage.h"
#include <string>

PersistentMatchStage::PersistentMatchStage(sc_module_name nm, pfp::core::PFPObject* parent,std::string configfile ):PersistentMatchStageSIM(nm,parent,configfile) {  // NOLINT(whitespace/line_length)
  /*sc_spawn threads*/
}

void PersistentMatchStage::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}
void PersistentMatchStage::PersistentMatchStage_PortServiceThread() {
  // Thread function to service input ports.
}
void PersistentMatchStage::PersistentMatchStageThread(std::size_t thread_id) {
  // Thread function for module functionalty
}

void PersistentMatchStage::configure() {
  has_config = true;
}