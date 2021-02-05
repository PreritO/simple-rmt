#include "OutputLogger.h"
#include <string>
#include <vector>
#include "PacketHeaderVector.h"
#include "common/PcapLogger.h"

OutputLogger::OutputLogger(sc_module_name nm,
      pfp::core::PFPObject* parent,
      std::string configfile):OutputLoggerSIM(nm, parent, configfile),outlog(OUTPUTDIR+"EgressTrace.csv") {
  /*sc_spawn threads*/
  ThreadHandles.push_back(sc_spawn(sc_bind(
        &OutputLogger::OutputLogger_PortServiceThread, this)));
}

void OutputLogger::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}

void OutputLogger::OutputLogger_PortServiceThread() {
  std::string outputfile = SPARG("validation-out");
  PcapLogger logger(outputfile);
  while (1) {
    if (!in->nb_can_get()) {
      wait(in->ok_to_get());
    } else {
      auto p = in->get();
      if (std::dynamic_pointer_cast<PacketHeaderVector>(p)) {
        std::shared_ptr<PacketHeaderVector> phv
              = std::dynamic_pointer_cast<PacketHeaderVector>(p);
        npulog(profile, cout << module_name() << " received packet "
              << phv->id() << endl;)
        npulog(normal, cout << module_name() << " received packet "
              << phv->id() << endl;)
        // added by PO to write time of logger
        outlog<<phv->id()<<","<<sc_time_stamp().to_default_time_units()<<endl;  // NOLINT
        std::vector<uint8_t> data;
        char *packet_data = phv->packet()->data();
        int data_size = phv->packet()->get_data_size();
        for (int i = 0; i < data_size; i++) {
          data.push_back(static_cast<uint8_t>(packet_data[i]));
        }
        logger.logPacket(data);
        // delete phv;
      } else {
        std::cerr << "Logger Packet Dynamic Cast Failed" << endl;
        SC_REPORT_ERROR("PACKET Logger", "Logger Dynamic Cast Failed");
        sc_stop();
      }
    }
  }
}

void OutputLogger::OutputLoggerThread(std::size_t thread_id) {
}