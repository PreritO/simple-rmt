#ifndef BEHAVIOURAL_DRAMTRIEWRAPPER_H_
#define BEHAVIOURAL_DRAMTRIEWRAPPER_H_
#include <vector>
#include <string>
#include "tries/src/Trie.h"
#include "DramSearchEngineS.h"

template <class T>
class DramTrieWrapper: public Trie<T> {
 public:
  // Constructor
  DramTrieWrapper();
  DramTrieWrapper(RoutingTableEntry<T> *iRoutingTable, int iRoutingTableSize,
        T iDefaultAction, int iDefaultActionSize);

  // Destructor
  virtual ~DramTrieWrapper();

  // Update
  virtual void update(RoutingTableEntry<T> *iRoutingTable,
        int iRoutingTableSize, typename Trie<T>::Action iAction);

  // Lookup
  virtual T exactPrefixMatch(BitString iPrefix) const;
  virtual T longestPrefixMatch(BitString iPrefix) const;

 private:
  void resolve_ports();

  sc_port<DramSearchEngineS> *dse_port = nullptr;
  sc_object* current_scmodule;
};

template <class T>
DramTrieWrapper<T>::DramTrieWrapper() {
  resolve_ports();
}

template<class T>
DramTrieWrapper<T>::DramTrieWrapper(RoutingTableEntry<T> *iRoutingTable,
      int iRoutingTableSize, T iDefaultAction, int iDefaultActionSize)
      : DramTrieWrapper() {
  (*dse_port)->setDefaultAction(new DramAction<T>(iDefaultAction));
  wait(1, SC_NS); //has no impact on throughput (as it shouldn't because only called when populating entries in hash table)
  RoutingTableEntry<DramActionBase*> *wTable
        = new RoutingTableEntry<DramActionBase*>[iRoutingTableSize];
  for (int i = 0; i < iRoutingTableSize; i++) {
    wTable[i].setData(iRoutingTable[i].getData());
    wTable[i].setLength(iRoutingTable[i].getLength());
    wTable[i].setAction(new DramAction<T>(iRoutingTable[i].getAction()));
    wTable[i].setActionSize(iRoutingTable[i].getActionSize());
  }
  (*dse_port)->insert(wTable, iRoutingTableSize);
  delete [] wTable;
}

template <class T>
DramTrieWrapper<T>::~DramTrieWrapper() {}

template <class T>
void DramTrieWrapper<T>::update(RoutingTableEntry<T> *iRoutingTable,
        int iRoutingTableSize, typename Trie<T>::Action iAction) {
  if (iAction == Trie<T>::Action::Add) {
    RoutingTableEntry<DramActionBase*> *wTable
          = new RoutingTableEntry<DramActionBase*>[iRoutingTableSize];
    for (int i = 0; i < iRoutingTableSize; i++) {
      wTable[i].setData(iRoutingTable[i].getData());
      wTable[i].setLength(iRoutingTable[i].getLength());
      wTable[i].setAction(new DramAction<T>(iRoutingTable[i].getAction()));
      wTable[i].setActionSize(iRoutingTable[i].getActionSize());
    }
    (*dse_port)->insert(wTable, iRoutingTableSize);
    //npulog(profile, cout << "ADD ACTION ENTRY -  Size: " << actionSize << endl;) 
    delete [] wTable;
  } else if (iAction == Trie<T>::Action::Remove) {
    for (int i = 0; i < iRoutingTableSize; i++) {
      (*dse_port)->remove(iRoutingTable[i].getData());
    }
  } else if (iAction == Trie<T>::Action::Reconstruct) {
    RoutingTableEntry<DramActionBase*> *wTable
          = new RoutingTableEntry<DramActionBase*>[iRoutingTableSize];
    for (int i = 0; i < iRoutingTableSize; i++) {
      wTable[i].setData(iRoutingTable[i].getData());
      wTable[i].setLength(iRoutingTable[i].getLength());
      wTable[i].setAction(new DramAction<T>(iRoutingTable[i].getAction()));
      wTable[i].setActionSize(iRoutingTable[i].getActionSize());
    }
    (*dse_port)->reconstruct(wTable, iRoutingTableSize);
    delete [] wTable;
  }
}

template <class T>
T DramTrieWrapper<T>::exactPrefixMatch(BitString iPrefix) const {
  DramActionBase* action_base = (*dse_port)->exactSearch(iPrefix);
  DramAction<T>* action = static_cast<DramAction<T>*>(action_base);
  return action->getAction();
}

template <class T>
T DramTrieWrapper<T>::longestPrefixMatch(BitString iPrefix) const {
  DramActionBase* action_base = (*dse_port)->search(iPrefix);
  DramAction<T>* action = static_cast<DramAction<T>*>(action_base);
  return action->getAction();
}

template<class T>
void DramTrieWrapper<T>::resolve_ports() {
  sc_process_handle this_process = sc_get_current_process_handle();
  current_scmodule = this_process.get_parent_object();
  std::vector<sc_object *> children = current_scmodule->get_child_objects();
  sc_port<DramSearchEngineS> *dse_portt = nullptr;

  for (auto& each_child : children) {
    if (dynamic_cast< sc_port<DramSearchEngineS> *>(each_child)) {
      dse_portt = dynamic_cast< sc_port<DramSearchEngineS> *>(each_child);
      std::string portname = dse_portt->basename();
      if (portname.find("port_0") != std::string::npos) {
        dse_port = dynamic_cast< sc_port<DramSearchEngineS> *>(each_child);
      }
      // npulog(std::cout << "dse Port: " << dse_portt->kind() << std::endl;)
    }
  }
}

#endif  // BEHAVIOURAL_DRAMTRIEWRAPPER_H_
