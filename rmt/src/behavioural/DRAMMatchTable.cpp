#include "DRAMMatchTable.h"
#include <string>
#include "MatchStage.h"
#include "PacketHeaderVector.h"
#include "RMTMessages.h"

DRAMMatchTable::DRAMMatchTable(sc_module_name nm, pfp::core::PFPObject* parent,
      std::string configfile):DRAMMatchTableSIM(nm, parent, configfile), outlog(OUTPUTDIR+"DRAMLookup.csv") {
  /*sc_spawn threads*/

  //original scenario here, no asynch processing...
  ThreadHandles.push_back(sc_spawn(sc_bind(&DRAMMatchTable::DRAMMatchTableOriginalThread,
        this, 0)));
  ThreadHandles.push_back(sc_spawn(sc_bind(&DRAMMatchTable::DRAMMatchAsyncLookupThread,
        this, 1)));
  ThreadHandles.push_back(sc_spawn(sc_bind(&DRAMMatchTable::DRAMMatchAsyncResponseThread,
        this, 2)));

  pktTxRate = GetParameter("tx_rate").get();
  if (pktTxRate == 0) {
    SC_REPORT_ERROR("VLIW Constructor", "Invalid tx rate configuration parameter");
  }
}

void DRAMMatchTable::init() {
    init_SIM(); /* Calls the init of sub PE's and CE's */
    global_table = "nat";

}

// use this thread to handle all incoming pkts and adding them to a 
// local fifo queue so we don't delay adding the packets from the selector in the first place while lookups
// are being performed
void DRAMMatchTable::DRAMMatchTable_PortServiceThread() {

}

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
                        
                        npulog(profile, std::cout << module_stack
                                    << " performing lookup on packet " << phv->id() << " ("
                                    << global_table << ")"<< std::endl;)
                        npulog(normal, std::cout << module_stack
                              << " performing lookup on packet " << phv->id() << " ("
                              << global_table << ")"<< std::endl;)
                        
                        async_queue.push(phv);
                        async_rx.notify();

                        // helps regulate the throughpt 
                        wait(1/(pktTxRate*1.0), SC_NS);
                        // // Write packet
                        // npulog(profile, std::cout << module_stack << " wrote packet "
                        //       << phv->id() << std::endl;)
                        // idea is that I write the packet out, and then perform the lookup
                        // not on critical path (similar to rdma request/response idealogy)
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

void DRAMMatchTable::DRAMMatchAsyncLookupThread(std::size_t thread_id) {
      while(1) {
            // this wait here is being grouped when packets in above thread
            // are constantly notifying, so we need to clear the queue after
            // this wait..
            wait(async_rx);
            int qsize_rx;
            async_queue.size(qsize_rx);
            while(qsize_rx > 0 ) {
                  auto phv = async_queue.pop();
                  // this performs the actual lookup - so let's time here to get 
                  // end-to-end latency
                  outlog<<phv->id()<<","<<sc_time_stamp().to_default_time_units()<<",";  // NOLINT

                  bm::ControlFlowNode* control_flow_node = P4::get("rmt")->
                              get_p4_objects()->get_control_node(global_table);

                  // note that as is with exact match tble entries, if entry doesn't exist in this match table
                  // the packet will not match and so you shouldn't see the lookup latency if pfpdebugger
                  const bm::ControlFlowNode* next_control_flow_node = (*control_flow_node) (phv->packet().get());

                  outlog<<sc_time_stamp().to_default_time_units()<<endl;  // NOLINT

                  async_response_queue.push(phv);
                  async_response.notify();
            }
      }
}

void DRAMMatchTable::DRAMMatchAsyncResponseThread(std::size_t thread_id) { 
      while(1) {
            /// again, this wait here is being called once for multiple
            // packets, and so deal with that by checking how ever many
            // packets are queued up..
            wait(async_response);
            int qsize_resp;
            async_response_queue.size(qsize_resp);
            while(qsize_resp > 0) {
                  auto phv = async_response_queue.pop();
                  npulog(profile, std::cout << "DRAM wrote packet "
                        << phv->id() << std::endl;)
            }
      }
}