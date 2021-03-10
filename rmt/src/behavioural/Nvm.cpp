#include "Nvm.h"
#include <string>

Nvm::Nvm(sc_module_name nm,
       pfp::core::PFPObject* parent, std::string configfile)
      :NvmSIM(nm, parent, configfile) {
  /*sc_spawn threads*/
  std::string delay_unit;
  int delay_time;
  try {
    nvm_height = static_cast<int>(GetParameter("height").get());
    nvm_width = static_cast<int>(GetParameter("width").get());
    delay_time = static_cast<int>(GetParameter("delay_time").get());
    delay_unit = GetParameter("delay_unit").get();
  } catch (std::exception& e) {
    SC_REPORT_ERROR("Nvm Constructor", "Invalid configuration paramter");
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
    SC_REPORT_ERROR("Nvm Constructor",
        "Invalid Nvm delay unit! Use SystemC sc_time_unit.");
  }
  nvm_delay = sc_time(delay_time, sc_unit);
}

void Nvm::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
}

void Nvm::Nvm_PortServiceThread() {
}

void Nvm::NvmThread(std::size_t thread_id) {
}

#include <bitset>
#define debug_nvm_mem_transaction false

/// ========================================
//
//  Search
//
/// ========================================

int Nvm::search(BitString prefix) {
  int index = 0;
  for (auto it = nvm_entries.begin(); it != nvm_entries.end(); it++) {
    if ((*it).getData() == prefix.substr(0, (*it).getData().size())
          && it->getValid() == true) {
        break;
    } else {
        index++;
    }
  }


  if (index == nvm_entries.size()) {
    index = -1;
  }
#if debug_nvm_mem_transaction
  cout << "      " << Nvm_name << " - Nvm_transaction = { S" << ", " << prefix
  << " } , result = " << hex << index << " at time " << sc_time_stamp() << endl;
#endif

  //wait(nvm_delay); PO - do we need this?
  return index;
}

int Nvm::exactSearch(BitString prefix) {
  int index = 0;
  for (auto it = nvm_entries.begin(); it != nvm_entries.end(); it++) {
    if (it->getData() == prefix && it->getValid() == true) {
      //wait(nvm_delay); // PO - again, do we need this?
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


void Nvm::insert(BitString prefix, unsigned int pos) {
  if (nvm_entries.size() + 1 > nvm_height) {
      throw "Unable to add entry. NVM size exceeded";
  } else if (prefix.size() > nvm_width) {
      throw "Unable to add entry. NVM width exceeded";
  }

  sc_time waitTime(0, SC_NS);

  if (pos < nvm_entries.size()) {
    auto it = nvm_entries.begin();
    advance(it, pos);
    NvmEntry entry(prefix);
    (*it) = entry;
    waitTime = nvm_delay;
  } else if (pos == nvm_entries.size()) {
    NvmEntry entry(prefix);
    nvm_entries.insert(nvm_entries.end(), entry);
    waitTime = nvm_delay;
  } else {
    int diff = pos - static_cast<int>(nvm_entries.size());
    for (int index = 0; index < diff; index++) {
        NvmEntry entry("", false);
        nvm_entries.insert(nvm_entries.end(), entry);
    }
    NvmEntry entry(prefix);
    nvm_entries.insert(nvm_entries.end(), entry);
    waitTime =(diff + 1)*nvm_delay;
  }

#if debug_nvm_mem_transaction
  cout << "      " << nvm_name << " - nvm_transaction = { W" << ", "
  << hex << pos << " } , data = " << dec << prefix << " at time "
  << sc_time_stamp() << " delay = " << nvm_delay << endl;
#endif

  //wait(waitTime);
}

/// ========================================
//
//  Insert and shift entries down
//
/// ========================================

void Nvm::insertAndShift(BitString prefix, unsigned int pos) {
  if (nvm_entries.size() + 1 > nvm_height) {
    throw "Unable to add entry. NVM size exceeded";
  } else if (prefix.size() > nvm_width) {
    throw "Unable to add entry. NVM width exceeded";
  }

  sc_time waitTime(0, SC_NS);

  if (pos < nvm_entries.size()) {
    auto it = nvm_entries.begin();
    advance(it, pos);
    NvmEntry entry(prefix);
    nvm_entries.insert(it, entry);
    //waitTime = (static_cast<int>(nvm_entries.size()) - pos)*nvm_delay;
    } else if (pos == nvm_entries.size()) {
        NvmEntry entry(prefix);
        nvm_entries.insert(nvm_entries.end(), entry);
        //waitTime = nvm_delay;
    } else {
      int diff = pos - static_cast<int>(nvm_entries.size());
      for (int index = 0; index < diff - 1; index++) {
        NvmEntry entry("", false);
        nvm_entries.insert(nvm_entries.end(), entry);
      }
        NvmEntry entry(prefix);
        nvm_entries.insert(nvm_entries.end(), entry);
        //waitTime = (diff + 1)*nvm_delay;
    }

#if debug_nvm_mem_transaction
  cout << "      " << nvm_name << " - nvm_transaction = { W (+ shift)"
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

void Nvm::remove(unsigned int pos) {
  if (pos < nvm_entries.size()) {
    auto it = nvm_entries.begin();
    advance(it, pos);
    it->setValid(false);
    //wait(nvm_delay);
    return;
  }
}

/// ========================================
//
//  Remove (and shift entries up)
//
/// ========================================

void Nvm::removeAndShift(unsigned int pos) {
  if (pos < nvm_entries.size()) {
    auto it = nvm_entries.begin();
    advance(it, pos);
    nvm_entries.erase(it);
    //wait((nvm_entries.size() - pos + 1)*nvm_delay);
  }
}

/// ========================================
//
//  Read - not on critical path for lookups, referrernced when nvmcontroller does insertions, manages DS
//
/// ========================================

BitString Nvm::read(unsigned int pos) {
  BitString result = "";

  if (pos < nvm_entries.size()) {
    auto it = nvm_entries.begin();
    advance(it, pos);
    if ((*it).getValid()) {
      result =  (*it).getData();
    }
  }

#if debug_nvm_mem_transaction
  cout << "      " << nvm_name<< " - nvm_transaction = { R, " << hex << pos
            << " } , data = " << result << " at time " << sc_time_stamp()
            << " delay = " << nvm_delay << endl;
#endif

  wait(nvm_delay); 
  return result;
}

/// ========================================
//
//  Get NVM size in bits
//
/// ========================================

int Nvm::getNvmSize() {
  return nvm_height*nvm_width;
}

BitString Nvm::DecimalToBinaryString(int64 iDecimal,
      const int iNumOfBits) const {
  BitString wString = std::bitset<64>(iDecimal).to_string();
  BitString wFinalString = wString.substr(wString.size() - iNumOfBits);
  return wFinalString;
}

/// ========================================
//
//  Returns the Nvm delay
//
/// ========================================

sc_time Nvm::getNvmDelay() {
  return nvm_delay;
}

/// ========================================
//
//  Functions for NvmEntry class
//
/// ========================================


NvmEntry::NvmEntry(BitString d, bool v): data(d), valid(v) {}

BitString NvmEntry::getData() const {
    return data;
}

bool NvmEntry::getValid() const {
    return valid;
}


void NvmEntry::setData(BitString d) {
    data = d;
}

void NvmEntry::setValid(bool v) {
    valid = v;
}
