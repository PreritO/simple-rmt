#include "DefaultNVMController.h"
#define debug_tlm_mem_transaction true
#include <bitset>
#include <algorithm>
#include <string>

DefaultNVMController::DefaultNVMController(sc_module_name nm,
      pfp::core::PFPObject* parent, std::string configfile)
      :DefaultNVMControllerSIM(nm, parent, configfile) {
  /*sc_spawn threads*/
}

void DefaultNVMController::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
    try {
        capacity = static_cast<int>(GetParameter("Capacity").get());
    } catch (std::exception& e) {
        SC_REPORT_ERROR("NVM Controller Constructor", "Invalid configuration parameter - Capacity");
    }
    actionTableSize = 0;
}
void DefaultNVMController::DefaultNVMController_PortServiceThread() {
}

void DefaultNVMController::DefaultNVMControllerThread(std::size_t thread_id) {
}

/// ================================
//
//  Insert
//
/// ================================

void DefaultNVMController::insert(BitString prefix, NVMActionBase* action) {
    int pos = 0;

    for (; pos < actionTableSize; pos++) {
        BitString temp = nvm_port->read(pos);
        if (temp == prefix) {
            nvm_port->insert(prefix, pos);
            write_mem(action, pos);
            return;
        }
        if (temp.size() < prefix.size()) {
            break;
        }
    }

    if (pos < actionTableSize) {
        for (int i = actionTableSize - 1; i >= pos; i--) {
            if (i < capacity) {
                NVMActionBase* val = read_mem(i);
                write_mem(val, i+1);
            } else {
              cout << "The Address was invalid in insert and shift" << endl;
            }
        }
        actionTableSize++;
    } else {
        actionTableSize += pos - actionTableSize + 1;
    }

    write_mem(action, pos);
    nvm_port->insertAndShift(prefix, pos);
}


void DefaultNVMController::insert(
      RoutingTableEntry<NVMActionBase*> *routingTable,
      unsigned int routingTableSize) {
    if (actionTableSize == 0) {
        reconstruct(routingTable, routingTableSize);
    } else {
        for (int i = 0; i < routingTableSize; i++) {
            insert(routingTable[i].getData(), routingTable[i].getAction());
        }
    }
}

/// ================================
//
//  Reconstruct
//
/// ================================

void DefaultNVMController::reconstruct(
      RoutingTableEntry<NVMActionBase*> *routingTable,
      unsigned int routingTableSize) {
    std::sort(routingTable, routingTable + routingTableSize, sortEntries);

    for (int i = 0; i < routingTableSize; i++) {
        nvm_port->insert(routingTable[i].getData(), i);
        write_mem(routingTable[i].getAction(), i);
    }

    for (int j = routingTableSize; j < actionTableSize; j++) {
        nvm_port->remove(j);
    }

    actionTableSize = routingTableSize;
}

/// ================================
//
//  Remove
//
/// ================================

void DefaultNVMController::remove(BitString prefix) {
    int pos = nvm_port->search(prefix);
    if (pos != -1) {
        nvm_port->removeAndShift(pos);

        for (int i = pos; i < actionTableSize - 1; i++) {
            NVMActionBase* val = read_mem(i + 1);
            write_mem(val, i);
        }

        actionTableSize--;
    }
}

/// ================================
//
//  Search
//
/// ================================

NVMActionBase* DefaultNVMController::exactSearch(BitString prefix) {
  int pos = nvm_port->exactSearch(prefix);
  if (pos == -1) {
    return defaultAction;
  } else {
    return read_mem(pos);
  }
}

NVMActionBase* DefaultNVMController::search(BitString prefix) {
    int pos = nvm_port->search(prefix);
    if (pos == -1) {
      return defaultAction;
    } else {
      NVMActionBase* action = read_mem(pos);
      return action;
    }
}

/// ================================
//
//  Sort Entries by Length
//
/// ================================

bool DefaultNVMController::sortEntries(
      RoutingTableEntry<NVMActionBase*> first,
      RoutingTableEntry<NVMActionBase*> second) {
    if (first.getLength() > second.getLength()) {
        return true;
    } else {
        return false;
    }
}

void DefaultNVMController::setDefaultAction(NVMActionBase* action) {
  defaultAction = action;
}

/*
 * ------------------- MEM R/W Functions
 *
 */

void DefaultNVMController::write_mem(NVMActionBase* data_to_allocate,
      int addr) {
  mem->write(data_to_allocate, addr);
}


NVMActionBase* DefaultNVMController::read_mem(int addr) {
  return mem->read(addr);
}
