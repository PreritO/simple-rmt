#include "./PersistentSelector.h"
#include <string>
#include <vector>
#include "PersistentMatchStage.h"

PersistentSelector::PersistentSelector(sc_module_name nm, pfp::core::PFPObject* parent,std::string configfile ):PersistentSelectorSIM(nm,parent,configfile) {  // NOLINT(whitespace/line_length)
  /*sc_spawn threads*/
  ThreadHandles.push_back(sc_spawn(sc_bind(&PersistentSelector::PersistentSelectorThread,
        this, 0)));
  pktTxRate = GetParameter("tx_rate").get();
  if (pktTxRate == 0) {
    SC_REPORT_ERROR("VLIW Constructor", "Invalid tx rate configuration parameter");
  }
}

void PersistentSelector::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}
void PersistentSelector::PersistentSelector_PortServiceThread() {
  // Thread function to service input ports.
}
void PersistentSelector::PersistentSelectorThread(std::size_t thread_id) {
  // Thread function for module functionalty
  PersistentMatchStage* parent = dynamic_cast<PersistentMatchStage*>(parent_);
  std::string module_stack = parent_->GetParent()->module_name() + "->"
        + parent_->module_name() + "->" + module_name();
  while(1) {
    if (!select_in->nb_can_get()) {
      wait(select_in->ok_to_get());
    } else {
      // Read input
      auto received = select_in->get();
      std::string received_type;
      if (received->data_type() == "PacketHeaderVector") {
        received_type = "packet";
      } else {
        received_type = "message";
      }
      npulog(profile, std::cout << module_stack << " Persistent SELECTOR: received "
            << received_type << " " << received->id() << " at " << sc_time_stamp().to_default_time_units() << std::endl;)
      wait(1/(pktTxRate*1.0), SC_NS);
      if (!select_out->nb_can_put()) {
        wait(select_out->ok_to_put());
      }
      npulog(profile, std::cout << module_stack << " SELECTOR: wrote " << received_type
            << " " << received->id() << " at " << sc_time_stamp().to_default_time_units() << std::endl;)
      select_out->put(received);
    }
  }
}
