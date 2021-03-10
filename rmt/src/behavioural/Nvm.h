#ifndef BEHAVIOURAL_NVM_H_
#define BEHAVIOURAL_NVM_H_
#include <list>
#include <vector>
#include <string>
#include "structural/NvmSIM.h"

class Nvm: public NvmSIM {
 public:
  SC_HAS_PROCESS(Nvm);
  /*Constructor*/
  Nvm(sc_module_name nm, pfp::core::PFPObject* parent = 0,
        std::string configfile = "");
  /*Destructor*/
  virtual ~Nvm() = default;

 public:
  void init();

  virtual void insert(BitString prefix, unsigned int pos);
  virtual void insertAndShift(BitString prefix, unsigned int pos);

  virtual int exactSearch(BitString prefix);
  virtual int search(BitString prefix);

  virtual void remove(unsigned int pos);
  virtual void removeAndShift(unsigned int pos);

  virtual BitString read(unsigned int pos);

  // Get Size of Nvm in Bits
  int getNvmSize();

  // GET Nvm Delay;
  sc_time getNvmDelay();

 private:
  void Nvm_PortServiceThread();
  void NvmThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;

  std::list<NvmEntry> nvm_entries;  // Nvm entries
  unsigned int nvm_height;  // Number of possible entries in nvm
  unsigned int nvm_width;  // Number of possible bits per entry
  sc_time nvm_delay;

  // Convert Decimal number to a binary string
  BitString DecimalToBinaryString(int64 iDecimal, const int iNumOfBits) const;
};

#endif  // BEHAVIOURAL_NVM_H_
