#include "DRAMMatchTable.h"
#include <string>
#include "MatchStage.h"
#include "PacketHeaderVector.h"
#include "RMTMessages.h"

DRAMMatchTable::DRAMMatchTable(sc_module_name nm, pfp::core::PFPObject* parent,
      std::string configfile):DRAMMatchTableSIM(nm, parent, configfile) {
  /*sc_spawn threads*/
  ThreadHandles.push_back(sc_spawn(sc_bind(&DRAMMatchTable::DRAMMatchTableThread,
        this, 0)));
  pktTxRate = GetParameter("tx_rate").get();
  if (pktTxRate == 0) {
    SC_REPORT_ERROR("VLIW Constructor", "Invalid tx rate configuration parameter");
  }
}

void DRAMMatchTable::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
}

void DRAMMatchTable::DRAMMatchTable_PortServiceThread() {
}

void DRAMMatchTable::DRAMMatchTableThread(std::size_t thread_id) {
  std::string module_stack = parent_->GetParent()->module_name() + "->"
        + parent_->module_name() + "->" + module_name();
  MatchStage* parent = dynamic_cast<MatchStage*>(parent_);
  while (1) {
      auto received_pkt = table_in->get();
      if (received_pkt->data_type() == "PacketHeaderVector") {
        std::shared_ptr<PacketHeaderVector> phv =
              std::dynamic_pointer_cast<PacketHeaderVector>(received_pkt);
        npulog(profile, std::cout << module_stack << " DRAM MATCH TABLE: received packet "
              << phv->id() << " at: " << sc_time_stamp().to_default_time_units() <<  std::endl;)

        // we don't care about stages here or anything, just preform the lookup from global table
        std::string stage_name = "nat";

        npulog(profile, std::cout << module_stack
                  << " performing lookup on packet " << phv->id() << " ("
                  << stage_name << ")"<< std::endl;)
        npulog(normal, std::cout << module_stack
              << " performing lookup on packet " << phv->id() << " ("
              << stage_name << ")"<< std::endl;)
      
      bm::ControlFlowNode* control_flow_node = P4::get("rmt")->
                  get_p4_objects()->get_control_node(stage_name);
      
      const bm::ControlFlowNode* next_control_flow_node =
            (*control_flow_node) (phv->packet().get());
      if (next_control_flow_node != 0) {
            std::string next_stage_name;
            const bm::MatchActionTable* match_table =
                  dynamic_cast<const bm::MatchActionTable*>
                  (next_control_flow_node);
      //       const bm::Conditional* cond =
      //       dynamic_cast<const bm::Conditional*>(next_control_flow_node);
            if (match_table != 0) {
                  next_stage_name = match_table->get_name();
            } 
      //      else if (cond != 0) {
      //       next_stage_name = cond->get_name();
      //       }
      //       npulog(profile, cout << module_stack
      //             << " setting next stage for packet " << phv->id() << " to "
      //             << next_stage_name << endl;)
            phv->set_next_table(next_stage_name);
      } else {
            npulog(profile, cout << module_stack
                  << " - no next stage for packet " << phv->id() << endl;)
            //phv->set_next_table("");
      }
      wait(1/(pktTxRate*1.0), SC_NS);
      // Write packet
      npulog(profile, std::cout << module_stack << " wrote packet "
            << phv->id() << std::endl;)
      table_out->put(received_pkt);
      } else {
        std::shared_ptr<RMTMessage> msg
              = std::dynamic_pointer_cast<RMTMessage>(received_pkt);
        npulog(profile, std::cout << module_stack << " received message "
              << received_pkt->id() << std::endl;)
        npulog(normal, std::cout << module_stack << " received message "
              << received_pkt->id() << std::endl;)
        // assume we only have one large table in Dram and not table per stage as was the case for sram
        msg->execute(*this);
      }
//     }
  }
}
