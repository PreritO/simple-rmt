#ifndef BEHAVIOURAL_DRAM_H_
#define BEHAVIOURAL_DRAM_H_
#include <list>
#include <vector>
#include <string>
#include "structural/DramSIM.h"

class Dram: public DramSIM {
 public:
  SC_HAS_PROCESS(Dram);
  /*Constructor*/
  Dram(sc_module_name nm, pfp::core::PFPObject* parent = 0,
        std::string configfile = "");
  /*Destructor*/
  virtual ~Dram() = default;

 public:
  void init();

  virtual void insert(BitString prefix, unsigned int pos);
  virtual void insertAndShift(BitString prefix, unsigned int pos);

  virtual int exactSearch(BitString prefix);
  virtual int search(BitString prefix);

  virtual void remove(unsigned int pos);
  virtual void removeAndShift(unsigned int pos);

  virtual BitString read(unsigned int pos);

  // Get Size of Dram in Bits
  int getDramSize();

  // GET Dram Delay;
  sc_time getDramDelay();

 private:
  void Dram_PortServiceThread();
  void DramThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;

  std::list<DramEntry> dram_entries;  // Dram entries
  unsigned int dram_height;  // Number of possible entries in dram
  unsigned int dram_width;  // Number of possible bits per entry
  sc_time dram_delay;

  // Convert Decimal number to a binary string
  BitString DecimalToBinaryString(int64 iDecimal, const int iNumOfBits) const;
};

#endif  // BEHAVIOURAL_DRAM_H_
