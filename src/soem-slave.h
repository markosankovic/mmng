#pragma once

#include <cstdint>

#include "soem.h"

class SoemSlave : public Slave {

  uint8_t position_;

public:
  SoemSlave(const uint8_t position) : position_(position) {}

  uint16_t get_state() override {
    return get_ethercat_slave_state(&ecx_context, position_, true);
  }

  SlaveInfo get_info() override {
    SlaveInfo info;
    info.position = position_;
    info.name = ec_slave[position_].name;
    info.state = get_state();
    return info;
  }
};
