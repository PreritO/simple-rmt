
#ifndef BEHAVIOURAL_DRAMS_H_
#define BEHAVIOURAL_DRAMS_H_

#include "systemc.h"
#include "tlm.h"
#include "tries/src/BitString.h"

using tlm::tlm_tag;

class DramEntry {
 public:
  explicit DramEntry(BitString d = "", bool v = true);

  // Getters
  BitString getData() const;
  bool getValid() const;

  // Setters
  void setData(BitString d);
  void setValid(bool v);

 private:
  BitString data;
  bool valid;
};

class DramS : public sc_interface {
 public:
  /* User Logic - Virtual Functions for interface go here */
  virtual void insert(BitString prefix, unsigned int pos) = 0;
  virtual void insertAndShift(BitString prefix, unsigned int pos) = 0;

  virtual int exactSearch(BitString prefix) = 0;
  virtual int search(BitString prefix) = 0;

  virtual void remove(unsigned int pos) = 0;
  virtual void removeAndShift(unsigned int pos) = 0;

  virtual BitString read(unsigned int pos) = 0;
};

#endif  // BEHAVIOURAL_DRAMS_H_
