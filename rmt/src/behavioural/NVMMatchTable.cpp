#include "NVMMatchTable.h"
#include <string>
#include "MatchStage.h"
#include "PacketHeaderVector.h"
#include "RMTMessages.h"

NVMMatchTable::NVMMatchTable(sc_module_name nm, pfp::core::PFPObject* parent,
      std::string configfile):NVMMatchTableSIM(nm, parent, configfile), outlog(OUTPUTDIR+"NVMLookup.csv") {
  /*sc_spawn threads*/

  //original scenario here, no asynch processing...
  ThreadHandles.push_back(sc_spawn(sc_bind(&NVMMatchTable::NVMMatchTableOriginalThread,
        this, 0)));

  pktTxRate = GetParameter("tx_rate").get();
  if (pktTxRate == 0) {
    SC_REPORT_ERROR("VLIW Constructor", "Invalid tx rate configuration parameter");
  }
}

void NVMMatchTable::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
    global_table = "nat";

}

// use this thread to handle all incoming pkts and adding them to a 
// local fifo queue so we don't delay adding the packets from the selector in the first place while lookups
// are being performed
void NVMMatchTable::NVMMatchTable_PortServiceThread() {

}

void NVMMatchTable::NVMMatchTableOriginalThread(std::size_t thread_id) {
      std::string module_stack = "NVM";
      while(1) {
            if (!table_in->nb_can_get() && !cp_agent_in->nb_can_get()) {
                  wait(table_in->ok_to_get() | cp_agent_in->ok_to_get());
            } else {
                  // read input
                  if (table_in->nb_can_get()) {
                        auto received_pkt = table_in->get();
                        if (received_pkt->data_type() == "PacketHeaderVector") { 
                              std::shared_ptr<PacketHeaderVector> phv =
                              std::dynamic_pointer_cast<PacketHeaderVector>(received_pkt);
                              npulog(profile, std::cout << module_stack << " received packet "
                                    << phv->id() << std::endl;)

                              // helps regulate the throughpt 
                              wait(1/(pktTxRate*1.0), SC_NS);

                              outlog<<phv->id()<<","<<sc_time_stamp().to_default_time_units()<<",";  // NOLINT
                              
                              npulog(profile, std::cout << module_stack
                                          << " performing lookup on packet " << phv->id() << " ("
                                          << global_table << ")"<< std::endl;)
                              npulog(normal, std::cout << module_stack
                                    << " performing lookup on packet " << phv->id() << " ("
                                    << global_table << ")"<< std::endl;)
                              // can no longer be off the critical path: 
                              bm::ControlFlowNode* control_flow_node = P4::get("rmt")->
                              get_p4_objects()->get_control_node(global_table);
                              // note that as is with exact match tble entries, if entry doesn't exist in this match table
                              // the packet will not match and so you shouldn't see the lookup latency if pfpdebugger
                              const bm::ControlFlowNode* next_control_flow_node = (*control_flow_node) (phv->packet().get());
                              outlog<<sc_time_stamp().to_default_time_units()<<endl;  // NOLINT
                                                            
                              // have to set it to true so backet can be forwarded from egress correctly
                              phv->setLookupState(true);

                              // npulog(profile, std::cout << "NVM wrote packet "<< phv->id() << std::endl;)

                              // // Write packet
                              npulog(profile, std::cout << module_stack << " wrote packet "
                                    << phv->id() << std::endl;)
                              // idea is that I write the packet out, and then perform the lookup
                              // not on critical path (similar to rdma request/response idealogy)
                              table_out->put(phv);
                        } 
                        // else {
                        //       // the following message SHOULD come from cp agent instead...
                        //       //SC_REPORT_ERROR("NVM Match Table", "Invalid Command in Table IN queue!");
                        //       npulog(profile, std::cout << module_stack << " Invalid Command in Table IN queue: "
                        //             << received_pkt->data_type() << std::endl;)
                        //       std::shared_ptr<RMTMessage> msg
                        //             = std::dynamic_pointer_cast<RMTMessage>(received_pkt);
                        //       npulog(profile, std::cout << module_stack << " received message "
                        //             << received_pkt->id() << std::endl;)
                        //       msg->execute(*this);
                        //       cp_agent_out->put(msg);
                        // }
                  } else if (cp_agent_in->nb_can_get()) {
                        auto received_pkt = cp_agent_in->get();
                        std::shared_ptr<RMTMessage> msg
                                    = std::dynamic_pointer_cast<RMTMessage>(received_pkt);
                        npulog(normal, std::cout << module_stack << " received message "
                              << received_pkt->id() << std::endl;)
                        npulog(profile, std::cout << module_stack << " received message "
                              << received_pkt->id() << std::endl;)
                        npulog(profile, std::cout << module_stack << " Message Data Type: "
                               << received_pkt->data_type() << std::endl;)
                        msg->execute(*this);
                        cp_agent_out->put(msg);
                  } else {
                        SC_REPORT_ERROR("NVM Match Table", "Impossible Condition!");
                  }
            }
      }
 }

void NVMMatchTable::NVMMatchAsyncLookupThread(std::size_t thread_id) {
      // while(1) {
      //       // this wait here is being grouped when packets in above thread
      //       // are constantly notifying, so we need to clear the queue after
      //       // this wait..
      //       wait(async_rx);
      //       int qsize_rx;
      //       async_queue.size(qsize_rx);
      //       while(qsize_rx > 0 ) {
      //             auto phv = async_queue.pop();
      //             // this performs the actual lookup - so let's time here to get 
      //             // end-to-end latency
      //             outlog<<phv->id()<<","<<sc_time_stamp().to_default_time_units()<<",";  // NOLINT

      //             bm::ControlFlowNode* control_flow_node = P4::get("rmt")->
      //                         get_p4_objects()->get_control_node(global_table);

      //             // note that as is with exact match tble entries, if entry doesn't exist in this match table
      //             // the packet will not match and so you shouldn't see the lookup latency if pfpdebugger
      //             const bm::ControlFlowNode* next_control_flow_node = (*control_flow_node) (phv->packet().get());

      //             outlog<<sc_time_stamp().to_default_time_units()<<endl;  // NOLINT

      //             async_response_queue.push(phv);
      //             async_response.notify();
      //       }
      // }
}

void NVMMatchTable::NVMMatchAsyncResponseThread(std::size_t thread_id) { 
      // while(1) {
      //       /// again, this wait here is being called once for multiple
      //       // packets, and so deal with that by checking how ever many
      //       // packets are queued up..
      //       wait(async_response);
      //       int qsize_resp;
      //       async_response_queue.size(qsize_resp);
      //       while(qsize_resp > 0) {
      //             auto phv = async_response_queue.pop();
      //             npulog(profile, std::cout << "NVM wrote packet "
      //                   << phv->id() << std::endl;)
      //       }
      // }
}