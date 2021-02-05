#ifndef BEHAVIOURAL_OUTPUTLOGGER_H_
#define BEHAVIOURAL_OUTPUTLOGGER_H_
#include <vector>
#include <string>
#include "structural/OutputLoggerSIM.h"
#include <fstream>

class OutputLogger: public OutputLoggerSIM {
 public:
  SC_HAS_PROCESS(OutputLogger);
  /*Constructor*/
  OutputLogger(sc_module_name nm, pfp::core::PFPObject* parent = 0,
        std::string configfile = "");
  /*Destructor*/
  virtual ~OutputLogger() = default;

 public:
  void init();

 private:
  void OutputLogger_PortServiceThread();
  void OutputLoggerThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
  std::ofstream outlog;
};
#endif  // BEHAVIOURAL_OUTPUTLOGGER_H_