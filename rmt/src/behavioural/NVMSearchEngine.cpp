#include "NVMSearchEngine.h"
#include <vector>
#include <string>

NVMSearchEngine::NVMSearchEngine(sc_module_name nm,
      pfp::core::PFPObject* parent, std::string configfile)
      :NVMSearchEngineSIM(nm, parent, configfile) {
    /*sc_spawn threads*/
}

void NVMSearchEngine::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
}

void NVMSearchEngine::NVMSearchEngine_PortServiceThread() {
}

void NVMSearchEngine::NVMSearchEngineThread(std::size_t thread_id) {
}

// Insert
void NVMSearchEngine::insert(BitString prefix, NVMActionBase* action) {
  nvm_controller->insert(prefix, action);
}

void NVMSearchEngine::insert(RoutingTableEntry<NVMActionBase*> *routingTable,
      unsigned int routingTableSize) {
  nvm_controller->insert(routingTable, routingTableSize);
}

// Remove
void NVMSearchEngine::remove(BitString prefix) {
  nvm_controller->remove(prefix);
}

// Reconstruct
void NVMSearchEngine::reconstruct(
      RoutingTableEntry<NVMActionBase*> *routingTable,
      unsigned int routingTableSize) {
  nvm_controller->reconstruct(routingTable, routingTableSize);
}

// Search
NVMActionBase* NVMSearchEngine::exactSearch(BitString prefix) {
  return nvm_controller->exactSearch(prefix);
}

NVMActionBase* NVMSearchEngine::search(BitString prefix) {
  return nvm_controller->search(prefix);
}

void NVMSearchEngine::setDefaultAction(NVMActionBase* action) {
  nvm_controller->setDefaultAction(action);
}
