#include "DefaultDramController.h"
#define debug_tlm_mem_transaction true
#include <bitset>
#include <algorithm>
#include <string>

DefaultDramController::DefaultDramController(sc_module_name nm,
      pfp::core::PFPObject* parent, std::string configfile)
      :DefaultDramControllerSIM(nm, parent, configfile) {
  /*sc_spawn threads*/
}

void DefaultDramController::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
    try {
        capacity = static_cast<int>(GetParameter("Capacity").get());
    } catch (std::exception& e) {
        SC_REPORT_ERROR("Dram Controller Constructor", "Invalid configuration parameter - Capacity");
    }
    actionTableSize = 0;
}
void DefaultDramController::DefaultDramController_PortServiceThread() {
}

void DefaultDramController::DefaultDramControllerThread(std::size_t thread_id) {
}

/// ================================
//
//  Insert
//
/// ================================

void DefaultDramController::insert(BitString prefix, DramActionBase* action) {
    int pos = 0;

    for (; pos < actionTableSize; pos++) {
        BitString temp = dram_port->read(pos);
        if (temp == prefix) {
            dram_port->insert(prefix, pos);
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
                DramActionBase* val = read_mem(i);
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
    dram_port->insertAndShift(prefix, pos);
}


void DefaultDramController::insert(
      RoutingTableEntry<DramActionBase*> *routingTable,
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

void DefaultDramController::reconstruct(
      RoutingTableEntry<DramActionBase*> *routingTable,
      unsigned int routingTableSize) {
    std::sort(routingTable, routingTable + routingTableSize, sortEntries);

    for (int i = 0; i < routingTableSize; i++) {
        dram_port->insert(routingTable[i].getData(), i);
        write_mem(routingTable[i].getAction(), i);
    }

    for (int j = routingTableSize; j < actionTableSize; j++) {
        dram_port->remove(j);
    }

    actionTableSize = routingTableSize;
}

/// ================================
//
//  Remove
//
/// ================================

void DefaultDramController::remove(BitString prefix) {
    int pos = dram_port->search(prefix);
    if (pos != -1) {
        dram_port->removeAndShift(pos);

        for (int i = pos; i < actionTableSize - 1; i++) {
            DramActionBase* val = read_mem(i + 1);
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

DramActionBase* DefaultDramController::exactSearch(BitString prefix) {
  int pos = dram_port->exactSearch(prefix);
  if (pos == -1) {
    return defaultAction;
  } else {
    return read_mem(pos);
  }
}

DramActionBase* DefaultDramController::search(BitString prefix) {
    int pos = dram_port->search(prefix);
    if (pos == -1) {
      return defaultAction;
    } else {
      DramActionBase* action = read_mem(pos);
      return action;
    }
}

/// ================================
//
//  Sort Entries by Length
//
/// ================================

bool DefaultDramController::sortEntries(
      RoutingTableEntry<DramActionBase*> first,
      RoutingTableEntry<DramActionBase*> second) {
    if (first.getLength() > second.getLength()) {
        return true;
    } else {
        return false;
    }
}

void DefaultDramController::setDefaultAction(DramActionBase* action) {
  defaultAction = action;
}

/*
 * ------------------- MEM R/W Functions
 *
 */

void DefaultDramController::write_mem(DramActionBase* data_to_allocate,
      int addr) {
  mem->write(data_to_allocate, addr);
}


DramActionBase* DefaultDramController::read_mem(int addr) {
  return mem->read(addr);
}
