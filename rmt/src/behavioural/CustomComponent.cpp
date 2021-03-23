#include "./CustomComponent.h"
#include <string>
#include "PacketHeaderVector.h"
#include "PacketPointer.h"


CustomComponent::CustomComponent(sc_module_name nm, pfp::core::PFPObject* parent,std::string configfile ):CustomComponentSIM(nm,parent,configfile) {  // NOLINT(whitespace/line_length)
  /*sc_spawn threads*/
  ThreadHandles.push_back(sc_spawn(sc_bind(
        &CustomComponent::CustomComponent_PortServiceThread, this)));
}

void CustomComponent::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}
void CustomComponent::CustomComponent_PortServiceThread() {
  std::string module_stack = parent_->module_name() + "->" + module_name();
  // Thread function to service input ports.
  while(1) {
    if (!input->nb_can_get()) {
      wait(input->ok_to_get());
    } else {
      auto received = input->get();
      if (received->data_type() == "PacketHeaderVector") {
        auto phv = std::dynamic_pointer_cast<PacketHeaderVector>(received);
        npulog(profile, std::cout << module_stack << " deparsed packet "
              << phv->id() << std::endl;)
        npulog(normal, std::cout << module_stack << " deparsed packet "
              << phv->id() << std::endl;)
        
        //check status of lookup here
        if(!phv->getLookupState()) {
          // forward to persistent pipeline here
          output_to_pp->put(received);
           npulog(normal, std::cout << module_stack << " forwarding packet "
              << phv->id() << " to Persistent Pipeline" << std::endl;)
        } else {
          npulog(normal, std::cout << module_stack << " forwarding packet "
              << phv->id() << " to Buffer" << std::endl;)
          output->put(received);
        }
      } else if(received->data_type() == "PacketPointer") {
        auto pp = std::dynamic_pointer_cast<PacketPointer>(received);
        auto phv = pp->phv;
        //check status of lookup here
        if(!phv->getLookupState()) {
          // forward to persistent pipeline here
          output_to_pp->put(received);
           npulog(normal, std::cout << module_stack << " forwarding packet "
              << phv->id() << " to Persistent Pipeline" << std::endl;)
        } else {
          npulog(normal, std::cout << module_stack << " forwarding packet "
              << phv->id() << " to Buffer" << std::endl;)
          output->put(received);
        }
      } else {
        npulog(profile, std::cout << module_stack << " Received arbitary message " << received->data_type() << std::endl;)
        npulog(normal, std::cout << module_stack << " Received arbitary message " << received->data_type() << std::endl;)
        output->put(received);
      }
    }
  }
}
void CustomComponent::CustomComponentThread(std::size_t thread_id) {
  // Thread function for module functionalty
}
