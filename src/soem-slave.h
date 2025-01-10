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

  bool set_state(uint16_t target_state) override {
    return set_ethercat_slave_state(&ecx_context, position_, target_state);
  }

  int upload(uint16_t index, uint8_t subindex) override { return 123; }
};
