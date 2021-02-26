#include "DramSearchEngine.h"
#include <vector>
#include <string>

DramSearchEngine::DramSearchEngine(sc_module_name nm,
      pfp::core::PFPObject* parent, std::string configfile)
      :DramSearchEngineSIM(nm, parent, configfile) {
    /*sc_spawn threads*/
}

void DramSearchEngine::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
}

void DramSearchEngine::DramSearchEngine_PortServiceThread() {
}

void DramSearchEngine::DramSearchEngineThread(std::size_t thread_id) {
}

// Insert
void DramSearchEngine::insert(BitString prefix, DramActionBase* action) {
  dram_controller->insert(prefix, action);
}

void DramSearchEngine::insert(RoutingTableEntry<DramActionBase*> *routingTable,
      unsigned int routingTableSize) {
  dram_controller->insert(routingTable, routingTableSize);
}

// Remove
void DramSearchEngine::remove(BitString prefix) {
  dram_controller->remove(prefix);
}

// Reconstruct
void DramSearchEngine::reconstruct(
      RoutingTableEntry<DramActionBase*> *routingTable,
      unsigned int routingTableSize) {
  dram_controller->reconstruct(routingTable, routingTableSize);
}

// Search
DramActionBase* DramSearchEngine::exactSearch(BitString prefix) {
  return dram_controller->exactSearch(prefix);
}

DramActionBase* DramSearchEngine::search(BitString prefix) {
  return dram_controller->search(prefix);
}

void DramSearchEngine::setDefaultAction(DramActionBase* action) {
  dram_controller->setDefaultAction(action);
}
