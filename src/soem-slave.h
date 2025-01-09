#pragma once

#include <cstdint>

#include "soem-master.h"

#include "soem.h"

class SoemSlave : public Slave {

  std::uint8_t position_;

public:
  SoemSlave(const std::uint8_t position) : position_(position) {}

  nlohmann::json to_json() const {
    nlohmann::json slave;
    slave["name"] = ec_slave[position_].name;
    slave["position"] = position_;
    return slave;
  }

  std::uint16_t get_state() override {
    return get_ethercat_slave_state(&ecx_context, position_, true);
  }
};
