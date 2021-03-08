#include "DRAMMatchTable.h"
#include <string>
#include "MatchStage.h"
#include "PacketHeaderVector.h"
#include "RMTMessages.h"

DRAMMatchTable::DRAMMatchTable(sc_module_name nm, pfp::core::PFPObject* parent,
      std::string configfile):DRAMMatchTableSIM(nm, parent, configfile) {
  /*sc_spawn threads*/
//   ThreadHandles.push_back(sc_spawn(sc_bind(&DRAMMatchTable::DRAMMatchTable_PortServiceThread,
//       this)));
//   ThreadHandles.push_back(sc_spawn(sc_bind(&DRAMMatchTable::DRAMMatchTablePHVThread,
//         this, 1)));
//   ThreadHandles.push_back(sc_spawn(sc_bind(&DRAMMatchTable::DRAMMatchTableRMTThread,
//         this, 2)));

  //original scenario here, no asynch processing...
  ThreadHandles.push_back(sc_spawn(sc_bind(&DRAMMatchTable::DRAMMatchTableOriginalThread,
        this, 0)));

  pktTxRate = GetParameter("tx_rate").get();
  if (pktTxRate == 0) {
    SC_REPORT_ERROR("VLIW Constructor", "Invalid tx rate configuration parameter");
  }
}

void DRAMMatchTable::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */

}

// use this thread to handle all incoming pkts and adding them to a 
// local fifo queue so we don't delay adding the packets from the selector in the first place while lookups
// are being performed
void DRAMMatchTable::DRAMMatchTable_PortServiceThread() {
      while(1) {
            auto received_pkt = table_in->get();
            if (received_pkt->data_type() == "PacketHeaderVector") {
                  phv_queue.push(std::dynamic_pointer_cast<PacketHeaderVector>(received_pkt));
                  phv_received.notify();
            } else {
                  rmt_queue.push(std::dynamic_pointer_cast<RMTMessage>(received_pkt));
                  rmt_received.notify();
            }

      }
}

void DRAMMatchTable::DRAMMatchTablePHVThread(std::size_t thread_id) {
  while (1) {
      wait(phv_received);
      std::string module_stack = parent_->GetParent()->module_name() + "->"
        + parent_->module_name() + "->" + module_name();
        
      auto phv = phv_queue.pop(); // treat in a FIFO manner
      int qsize;
      phv_queue.size(qsize);

      npulog(profile, std::cout << module_stack << " DRAM MATCH TABLE: received packet "
              << phv->id() << " at: " << sc_time_stamp().to_default_time_units() <<  std::endl;)

        // Todo:  make this a dynamic var - Prerit
        std::string global_table = "nat";
        npulog(profile, std::cout << module_stack
                  << " performing lookup on packet " << phv->id() << " ("
                  << global_table << ")"<< std::endl;)
        npulog(normal, std::cout << module_stack
              << " performing lookup on packet " << phv->id() << " ("
              << global_table << ")"<< std::endl;)
      
        bm::ControlFlowNode* control_flow_node = P4::get("rmt")->
                  get_p4_objects()->get_control_node(global_table);
      
        // this performs lookup
        const bm::ControlFlowNode* next_control_flow_node =
            (*control_flow_node) (phv->packet().get());

      //   // we don't care about subsequent stages, treat as a single stage
      //   if (next_control_flow_node != 0) {
      //       std::string next_stage_name;
      //       const bm::MatchActionTable* match_table =
      //             dynamic_cast<const bm::MatchActionTable*>
      //             (next_control_flow_node);
      //       const bm::Conditional* cond =
      //       dynamic_cast<const bm::Conditional*>(next_control_flow_node);
      //       if (match_table != 0) {
      //             next_stage_name = match_table->get_name();
      //       } 
      //      else if (cond != 0) {
      //       next_stage_name = cond->get_name();
      //       }
      //       npulog(profile, cout << module_stack
      //             << " setting next stage for packet " << phv->id() << " to "
      //             << next_stage_name << endl;)
      //       phv->set_next_table(next_stage_name);
      //   } else {
      //       npulog(profile, cout << module_stack
      //             << " - no next stage for packet " << phv->id() << endl;)
      //       phv->set_next_table("");
      //   }
        wait(1/(pktTxRate*1.0), SC_NS);
        // Write packet
        npulog(profile, std::cout << module_stack << " wrote packet "
            << phv->id() << std::endl;)
        table_out->put(phv);
  }
}

void DRAMMatchTable::DRAMMatchTableRMTThread(std::size_t thread_id) {
  std::string module_stack = parent_->GetParent()->module_name() + "->"
        + parent_->module_name() + "->" + module_name();
  MatchStage* parent = dynamic_cast<MatchStage*>(parent_);
  while (1) {
      wait(rmt_received);
      auto msg = rmt_queue.pop();
      npulog(profile, std::cout << module_stack << " received message "
              << msg->id() << std::endl;)
      npulog(normal, std::cout << module_stack << " received message "
              << msg->id() << std::endl;)
      // assume we only have one large table in Dram and not table per stage as was the case for sram
      msg->execute(*this);
      table_out->put(msg);
  }
}

      /*
      auto received_pkt = table_in->get();
      if (received_pkt->data_type() == "PacketHeaderVector") {
        std::shared_ptr<PacketHeaderVector> phv =
              std::dynamic_pointer_cast<PacketHeaderVector>(received_pkt);
        npulog(profile, std::cout << module_stack << " DRAM MATCH TABLE: received packet "
              << phv->id() << " at: " << sc_time_stamp().to_default_time_units() <<  std::endl;)

        // we don't care about stages here or anything, just preform the lookup from global table
        // Todo:  make this a dynamic var - Prerit
        std::string stage_name = "nat";

        npulog(profile, std::cout << module_stack
                  << " performing lookup on packet " << phv->id() << " ("
                  << stage_name << ")"<< std::endl;)
        npulog(normal, std::cout << module_stack
              << " performing lookup on packet " << phv->id() << " ("
              << stage_name << ")"<< std::endl;)
      
        bm::ControlFlowNode* control_flow_node = P4::get("rmt")->
                  get_p4_objects()->get_control_node(stage_name);
      
        // this performs lookup
        const bm::ControlFlowNode* next_control_flow_node =
            (*control_flow_node) (phv->packet().get());

        // we don't care about subsequent stages, treat as a single stage
        if (next_control_flow_node != 0) {
            std::string next_stage_name;
            const bm::MatchActionTable* match_table =
                  dynamic_cast<const bm::MatchActionTable*>
                  (next_control_flow_node);
            const bm::Conditional* cond =
            dynamic_cast<const bm::Conditional*>(next_control_flow_node);
            if (match_table != 0) {
                  next_stage_name = match_table->get_name();
            } 
           else if (cond != 0) {
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
        table_out->put(received_pkt);
      }
//     }
      */

void DRAMMatchTable::DRAMMatchTableOriginalThread(std::size_t thread_id) {
      std::string module_stack = parent_->GetParent()->module_name() + "->"
        + parent_->module_name() + "->" + module_name();
        
      while(1) {
            if (!table_in->nb_can_get()) {
                  wait(table_in->ok_to_get());
            } else {
                  // read input
                  auto received_pkt = table_in->get();
                  if (received_pkt->data_type() == "PacketHeaderVector") { 
                        std::shared_ptr<PacketHeaderVector> phv =
                        std::dynamic_pointer_cast<PacketHeaderVector>(received_pkt);
                        npulog(profile, std::cout << module_stack << " received packet "
                              << phv->id() << std::endl;)
                        
                        std::string global_table = "nat";
                        npulog(profile, std::cout << module_stack
                                    << " performing lookup on packet " << phv->id() << " ("
                                    << global_table << ")"<< std::endl;)
                        npulog(normal, std::cout << module_stack
                              << " performing lookup on packet " << phv->id() << " ("
                              << global_table << ")"<< std::endl;)
                        
                        bm::ControlFlowNode* control_flow_node = P4::get("rmt")->
                                    get_p4_objects()->get_control_node(global_table);
      
                        // this performs lookup
                        // note that as is with exact match tble entries, if entry doesn't exist in this match table
                        // the packet will not mtch and so you won't see the lookup latency if pfpdebugger
                        const bm::ControlFlowNode* next_control_flow_node = (*control_flow_node) (phv->packet().get());
                        
                        // ignore the whole conditional stuff, TEA assumes a massive single table..

                        // helps regulate the throughpt 
                        wait(1/(pktTxRate*1.0), SC_NS);
                        // Write packet
                        npulog(profile, std::cout << module_stack << " wrote packet "
                              << phv->id() << std::endl;)
                        table_out->put(phv);
                  } else {
                        std::shared_ptr<RMTMessage> msg
                              = std::dynamic_pointer_cast<RMTMessage>(received_pkt);
                        npulog(profile, std::cout << module_stack << " received message "
                              << received_pkt->id() << std::endl;)
                        msg->execute(*this);
                        table_out->put(msg);
                  }
            }
      }
 }