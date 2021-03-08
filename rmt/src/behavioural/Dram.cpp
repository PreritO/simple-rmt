#include "Dram.h"
#include <string>

Dram::Dram(sc_module_name nm,
       pfp::core::PFPObject* parent, std::string configfile)
      :DramSIM(nm, parent, configfile) {
  /*sc_spawn threads*/
  std::string delay_unit;
  int delay_time;
  try {
    dram_height = static_cast<int>(GetParameter("height").get());
    dram_width = static_cast<int>(GetParameter("width").get());
    delay_time = static_cast<int>(GetParameter("delay_time").get());
    delay_unit = GetParameter("delay_unit").get();
  } catch (std::exception& e) {
    SC_REPORT_ERROR("Dram Constructor", "Invalid configuration paramter");
  }
  sc_core::sc_time_unit sc_unit = SC_NS;
  if (delay_unit == "SC_FS") {
    sc_unit = SC_FS;
  } else if (delay_unit == "SC_PS") {
    sc_unit = SC_PS;
  } else if (delay_unit == "SC_NS") {
    sc_unit = SC_NS;
  } else if (delay_unit == "SC_US") {
    sc_unit = SC_US;
  } else if (delay_unit == "SC_MS") {
    sc_unit = SC_MS;
  } else if (delay_unit == "SC_SEC") {
    sc_unit = SC_SEC;
  } else {
    SC_REPORT_ERROR("Dram Constructor",
        "Invalid Dram delay unit! Use SystemC sc_time_unit.");
  }
  dram_delay = sc_time(delay_time, sc_unit);
}

void Dram::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
}

void Dram::Dram_PortServiceThread() {
}

void Dram::DramThread(std::size_t thread_id) {
}

#include <bitset>
#define debug_dram_mem_transaction false

/// ========================================
//
//  Search
//
/// ========================================

int Dram::search(BitString prefix) {
  int index = 0;
  for (auto it = dram_entries.begin(); it != dram_entries.end(); it++) {
    if ((*it).getData() == prefix.substr(0, (*it).getData().size())
          && it->getValid() == true) {
        break;
    } else {
        index++;
    }
  }


  if (index == dram_entries.size()) {
    index = -1;
  }
#if debug_dram_mem_transaction
  cout << "      " << Dram_name << " - Dram_transaction = { S" << ", " << prefix
  << " } , result = " << hex << index << " at time " << sc_time_stamp() << endl;
#endif

  //wait(dram_delay); PO - do we need this?
  return index;
}

int Dram::exactSearch(BitString prefix) {
  int index = 0;
  for (auto it = dram_entries.begin(); it != dram_entries.end(); it++) {
    if (it->getData() == prefix && it->getValid() == true) {
      //wait(dram_delay); // PO - again, do we need this?
      return index;
    } else {
      index++;
    }
  }
  return -1;
}

/// ========================================
//
//  Insert (no shift)
//
/// ========================================


void Dram::insert(BitString prefix, unsigned int pos) {
  if (dram_entries.size() + 1 > dram_height) {
      throw "Unable to add entry. DRAM size exceeded";
  } else if (prefix.size() > dram_width) {
      throw "Unable to add entry. DRAM width exceeded";
  }

  sc_time waitTime(0, SC_NS);

  if (pos < dram_entries.size()) {
    auto it = dram_entries.begin();
    advance(it, pos);
    DramEntry entry(prefix);
    (*it) = entry;
    waitTime = dram_delay;
  } else if (pos == dram_entries.size()) {
    DramEntry entry(prefix);
    dram_entries.insert(dram_entries.end(), entry);
    waitTime = dram_delay;
  } else {
    int diff = pos - static_cast<int>(dram_entries.size());
    for (int index = 0; index < diff; index++) {
        DramEntry entry("", false);
        dram_entries.insert(dram_entries.end(), entry);
    }
    DramEntry entry(prefix);
    dram_entries.insert(dram_entries.end(), entry);
    waitTime =(diff + 1)*dram_delay;
  }

#if debug_dram_mem_transaction
  cout << "      " << dram_name << " - dram_transaction = { W" << ", "
  << hex << pos << " } , data = " << dec << prefix << " at time "
  << sc_time_stamp() << " delay = " << dram_delay << endl;
#endif

  //wait(waitTime);
}

/// ========================================
//
//  Insert and shift entries down
//
/// ========================================

void Dram::insertAndShift(BitString prefix, unsigned int pos) {
  if (dram_entries.size() + 1 > dram_height) {
    throw "Unable to add entry. DRAM size exceeded";
  } else if (prefix.size() > dram_width) {
    throw "Unable to add entry. DRAM width exceeded";
  }

  sc_time waitTime(0, SC_NS);

  if (pos < dram_entries.size()) {
    auto it = dram_entries.begin();
    advance(it, pos);
    DramEntry entry(prefix);
    dram_entries.insert(it, entry);
    //waitTime = (static_cast<int>(dram_entries.size()) - pos)*dram_delay;
    } else if (pos == dram_entries.size()) {
        DramEntry entry(prefix);
        dram_entries.insert(dram_entries.end(), entry);
        //waitTime = dram_delay;
    } else {
      int diff = pos - static_cast<int>(dram_entries.size());
      for (int index = 0; index < diff - 1; index++) {
        DramEntry entry("", false);
        dram_entries.insert(dram_entries.end(), entry);
      }
        DramEntry entry(prefix);
        dram_entries.insert(dram_entries.end(), entry);
        //waitTime = (diff + 1)*dram_delay;
    }

#if debug_dram_mem_transaction
  cout << "      " << dram_name << " - dram_transaction = { W (+ shift)"
  << ", " << hex << pos << " } , data = " << dec << prefix << " at time "
  << sc_time_stamp() << " delay = " << waitTime << endl;
#endif

  //wait(waitTime);
}

/// ========================================
//
//  Remove
//
/// ========================================

void Dram::remove(unsigned int pos) {
  if (pos < dram_entries.size()) {
    auto it = dram_entries.begin();
    advance(it, pos);
    it->setValid(false);
    //wait(dram_delay);
    return;
  }
}

/// ========================================
//
//  Remove (and shift entries up)
//
/// ========================================

void Dram::removeAndShift(unsigned int pos) {
  if (pos < dram_entries.size()) {
    auto it = dram_entries.begin();
    advance(it, pos);
    dram_entries.erase(it);
    //wait((dram_entries.size() - pos + 1)*dram_delay);
  }
}

/// ========================================
//
//  Read - not on critical path for lookups, referrernced when dramcontroller does insertions, manages DS
//
/// ========================================

BitString Dram::read(unsigned int pos) {
  BitString result = "";

  if (pos < dram_entries.size()) {
    auto it = dram_entries.begin();
    advance(it, pos);
    if ((*it).getValid()) {
      result =  (*it).getData();
    }
  }

#if debug_dram_mem_transaction
  cout << "      " << dram_name<< " - dram_transaction = { R, " << hex << pos
            << " } , data = " << result << " at time " << sc_time_stamp()
            << " delay = " << dram_delay << endl;
#endif

  wait(dram_delay); 
  return result;
}

/// ========================================
//
//  Get DRAM size in bits
//
/// ========================================

int Dram::getDramSize() {
  return dram_height*dram_width;
}

BitString Dram::DecimalToBinaryString(int64 iDecimal,
      const int iNumOfBits) const {
  BitString wString = std::bitset<64>(iDecimal).to_string();
  BitString wFinalString = wString.substr(wString.size() - iNumOfBits);
  return wFinalString;
}

/// ========================================
//
//  Returns the Dram delay
//
/// ========================================

sc_time Dram::getDramDelay() {
  return dram_delay;
}

/// ========================================
//
//  Functions for DramEntry class
//
/// ========================================


DramEntry::DramEntry(BitString d, bool v): data(d), valid(v) {}

BitString DramEntry::getData() const {
    return data;
}

bool DramEntry::getValid() const {
    return valid;
}


void DramEntry::setData(BitString d) {
    data = d;
}

void DramEntry::setValid(bool v) {
    valid = v;
}
