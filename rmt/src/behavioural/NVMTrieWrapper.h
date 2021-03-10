#ifndef BEHAVIOURAL_NVMTRIEWRAPPER_H_
#define BEHAVIOURAL_NVMTRIEWRAPPER_H_
#include <vector>
#include <string>
#include "tries/src/Trie.h"
#include "NVMSearchEngineS.h"

template <class T>
class NVMTrieWrapper: public Trie<T> {
 public:
  // Constructor
  NVMTrieWrapper();
  NVMTrieWrapper(RoutingTableEntry<T> *iRoutingTable, int iRoutingTableSize,
        T iDefaultAction, int iDefaultActionSize);

  // Destructor
  virtual ~NVMTrieWrapper();

  // Update
  virtual void update(RoutingTableEntry<T> *iRoutingTable,
        int iRoutingTableSize, typename Trie<T>::Action iAction);

  // Lookup
  virtual T exactPrefixMatch(BitString iPrefix) const;
  virtual T longestPrefixMatch(BitString iPrefix) const;

 private:
  void resolve_ports();

  sc_port<NVMSearchEngineS> *nse_port = nullptr;
  sc_object* current_scmodule;
};

template <class T>
NVMTrieWrapper<T>::NVMTrieWrapper() {
  resolve_ports();
}

template<class T>
NVMTrieWrapper<T>::NVMTrieWrapper(RoutingTableEntry<T> *iRoutingTable,
      int iRoutingTableSize, T iDefaultAction, int iDefaultActionSize)
      : NVMTrieWrapper() {
  if (nse_port == nullptr) {
    SC_REPORT_ERROR("NVM Trie Wrapper", "NSE Port is NULL");
  }
  (*nse_port)->setDefaultAction(new NVMAction<T>(iDefaultAction));
  wait(1, SC_NS); //has no impact on throughput (as it shouldn't because only called when populating entries in hash table)
  RoutingTableEntry<NVMActionBase*> *wTable
        = new RoutingTableEntry<NVMActionBase*>[iRoutingTableSize];
  for (int i = 0; i < iRoutingTableSize; i++) {
    wTable[i].setData(iRoutingTable[i].getData());
    wTable[i].setLength(iRoutingTable[i].getLength());
    wTable[i].setAction(new NVMAction<T>(iRoutingTable[i].getAction()));
    wTable[i].setActionSize(iRoutingTable[i].getActionSize());
  }
  (*nse_port)->insert(wTable, iRoutingTableSize);
  delete [] wTable;
}

template <class T>
NVMTrieWrapper<T>::~NVMTrieWrapper() {}

template <class T>
void NVMTrieWrapper<T>::update(RoutingTableEntry<T> *iRoutingTable,
        int iRoutingTableSize, typename Trie<T>::Action iAction) {
  if (iAction == Trie<T>::Action::Add) {
    RoutingTableEntry<NVMActionBase*> *wTable
          = new RoutingTableEntry<NVMActionBase*>[iRoutingTableSize];
    for (int i = 0; i < iRoutingTableSize; i++) {
      wTable[i].setData(iRoutingTable[i].getData());
      wTable[i].setLength(iRoutingTable[i].getLength());
      wTable[i].setAction(new NVMAction<T>(iRoutingTable[i].getAction()));
      wTable[i].setActionSize(iRoutingTable[i].getActionSize());
    }
    (*nse_port)->insert(wTable, iRoutingTableSize);
    delete [] wTable;
  } else if (iAction == Trie<T>::Action::Remove) {
    for (int i = 0; i < iRoutingTableSize; i++) {
      (*nse_port)->remove(iRoutingTable[i].getData());
    }
  } else if (iAction == Trie<T>::Action::Reconstruct) {
    RoutingTableEntry<NVMActionBase*> *wTable
          = new RoutingTableEntry<NVMActionBase*>[iRoutingTableSize];
    for (int i = 0; i < iRoutingTableSize; i++) {
      wTable[i].setData(iRoutingTable[i].getData());
      wTable[i].setLength(iRoutingTable[i].getLength());
      wTable[i].setAction(new NVMAction<T>(iRoutingTable[i].getAction()));
      wTable[i].setActionSize(iRoutingTable[i].getActionSize());
    }
    (*nse_port)->reconstruct(wTable, iRoutingTableSize);
    delete [] wTable;
  }
}

template <class T>
T NVMTrieWrapper<T>::exactPrefixMatch(BitString iPrefix) const {
  NVMActionBase* action_base = (*nse_port)->exactSearch(iPrefix);
  NVMAction<T>* action = static_cast<NVMAction<T>*>(action_base);
  return action->getAction();
}

template <class T>
T NVMTrieWrapper<T>::longestPrefixMatch(BitString iPrefix) const {
  NVMActionBase* action_base = (*nse_port)->search(iPrefix);
  NVMAction<T>* action = static_cast<NVMAction<T>*>(action_base);
  return action->getAction();
}

template<class T>
void NVMTrieWrapper<T>::resolve_ports() {
  sc_process_handle this_process = sc_get_current_process_handle();
  current_scmodule = this_process.get_parent_object();
  std::vector<sc_object *> children = current_scmodule->get_child_objects();
  sc_port<NVMSearchEngineS> *nse_sse_portt = nullptr;

  for (auto& each_child : children) {
    if (dynamic_cast< sc_port<NVMSearchEngineS> *>(each_child)) {
      nse_sse_portt = dynamic_cast< sc_port<NVMSearchEngineS> *>(each_child);
      std::string portname = nse_sse_portt->basename();
      if (portname.find("port_0") != std::string::npos) {
        nse_port = dynamic_cast< sc_port<NVMSearchEngineS> *>(each_child);
      }
      // npulog(profile, cout << "nse Port: " << nse_sse_portt->kind() << endl;)
      // npulog(profile, cout << this_proc->name() << " got read lock@" << this
      //   << endl;)
    }
  }
}

#endif  // BEHAVIOURAL_NVMTRIEWRAPPER_H_
