/*
 * simple-rmt: Example RMT simulation model using the PFPSim Framework
 *
 * Copyright (C) 2016 Concordia Univ., Montreal
 *     Samar Abdi
 *     Umair Aftab
 *     Gordon Bailey
 *     Faras Dewal
 *     Shafigh Parsazad
 *     Eric Tremblay
 *
 * Copyright (C) 2016 Ericsson
 *     Bochra Boughzala
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef BEHAVIOURAL_PARSER_H_
#define BEHAVIOURAL_PARSER_H_
#include <string>
#include <vector>
#include "structural/ParserSIM.h"

class Parser: public ParserSIM {
 public:
  SC_HAS_PROCESS(Parser);
  /*Constructor*/
  Parser(sc_module_name nm, pfp::core::PFPObject* parent = 0,
        std::string configfile = "");
  /*Destructor*/
  virtual ~Parser() = default;

 public:
  void init();
  int pktTxRate;
  void set_first_table(std::string table);

 private:
  void Parser_PortServiceThread();
  void ParserThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;

//! first table or conditional to apply to packet in pipeline
  std::string first_table;
};

#endif  // BEHAVIOURAL_PARSER_H_
