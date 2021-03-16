#ifndef BEHAVIOURAL_CUSTOMCOMPONENT_H_
#define BEHAVIOURAL_CUSTOMCOMPONENT_H_
#include <string>
#include <vector>
#include "../structural/CustomComponentSIM.h"

class CustomComponent: public CustomComponentSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(CustomComponent);
  /*Constructor*/
  explicit CustomComponent(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~CustomComponent() = default;

 public:
  void init();

 private:
  void CustomComponent_PortServiceThread();
  void CustomComponentThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
};

#endif  // BEHAVIOURAL_CUSTOMCOMPONENT_H_
