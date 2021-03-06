#include "SRAMMatchTable.h"
#include <string>
#include "MatchStage.h"
#include "PacketHeaderVector.h"
#include "RMTMessages.h"

SRAMMatchTable::SRAMMatchTable(sc_module_name nm, pfp::core::PFPObject* parent,
      std::string configfile):SRAMMatchTableSIM(nm, parent, configfile) {
  /*sc_spawn threads*/
  ThreadHandles.push_back(sc_spawn(sc_bind(&SRAMMatchTable::SRAMMatchTableThread,
        this, 0)));
  pktTxRate = GetParameter("tx_rate").get();
  if (pktTxRate == 0) {
    SC_REPORT_ERROR("VLIW Constructor", "Invalid tx rate configuration parameter");
  }
}

void SRAMMatchTable::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
}

void SRAMMatchTable::SRAMMatchTable_PortServiceThread() {
}

void SRAMMatchTable::SRAMMatchTableThread(std::size_t thread_id) {
  std::string module_stack = parent_->GetParent()->module_name() + "->"
        + parent_->module_name() + "->" + module_name();
  MatchStage* parent = dynamic_cast<MatchStage*>(parent_);
  while (1) {
      auto received_pkt = table_in->get();
      if (received_pkt->data_type() == "PacketHeaderVector") {
        std::shared_ptr<PacketHeaderVector> phv =
              std::dynamic_pointer_cast<PacketHeaderVector>(received_pkt);
        npulog(profile, std::cout << module_stack << " SRAM MATCH TABLE: received packet "
              << phv->id() << " at: " << sc_time_stamp().to_default_time_units() <<  std::endl;)

        // Check if this stage is configured
        if (parent->has_config) {
          MatchStageConfig &config = parent->config;
          std::string stage_name = config.name;
          npulog(debug, cout << module_stack << " has a config. Stage Name: "
                << stage_name << endl;)
          // Check if the phv must be applied to this stage
          if (phv->get_next_table() != ""
                && stage_name == phv->get_next_table()) {
            // applying packet to match stage
            bm::ControlFlowNode* control_flow_node = P4::get("rmt")->
                  get_p4_objects()->get_control_node(stage_name);
            npulog(profile, std::cout << module_stack
                  << " performing lookup on packet " << phv->id() << " ("
                  << stage_name << ")"<< std::endl;)
            npulog(normal, std::cout << module_stack
                  << " performing lookup on packet " << phv->id() << " ("
                  << stage_name << ")"<< std::endl;)
            const bm::ControlFlowNode* next_control_flow_node =
                  (*control_flow_node)(phv->packet().get());
            if (next_control_flow_node != 0) {
              std::string next_stage_name;
              const bm::MatchActionTable* match_table =
                    dynamic_cast<const bm::MatchActionTable*>
                    (next_control_flow_node);
              const bm::Conditional* cond =
                  dynamic_cast<const bm::Conditional*>(next_control_flow_node);
              if (match_table != 0) {
                next_stage_name = match_table->get_name();
              } else if (cond != 0) {
                next_stage_name = cond->get_name();
              }
              npulog(profile, cout << module_stack
                    << " setting next stage for packet " << phv->id() << " to "
                    << next_stage_name << endl;)
              phv->set_next_table(next_stage_name);
            } else {
              npulog(profile, cout << module_stack
                    << " - no next stage for packet " << phv->id() << endl;)
              phv->set_next_table("");
            }
            wait(1/(pktTxRate*1.0), SC_NS);
          } else {
            npulog(profile, cout << module_stack
                  << " is not the next stage for packet " << phv->id()
                  << ". Writing packet to output port." << endl;)
          }
        } else {
          npulog(profile, cout << module_stack
                << " is an empty stage. Writing packet to output port."
                << endl;)
        }
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
        if (parent->has_config) {
          MatchStageConfig &config = parent->config;
          std::string stage_name = config.name;
          if (msg->table_name == stage_name) {
            npulog(profile, std::cout << module_stack << " executing message "
                  << received_pkt->id() << " (" << msg->table_name << ")"
                  << std::endl;)
            npulog(normal, std::cout << module_stack << " executing message "
                  << received_pkt->id() << " (" << msg->table_name << ")"
                  << std::endl;)
            msg->execute(*this);
          } else {
            // Message is not for this table
            table_out->put(received_pkt);
          }
        } else {
          npulog(debug, std::cout << module_stack << " is not configured!"
                << std::endl;)
          table_out->put(received_pkt);
        }
      }
//     }
  }
}
