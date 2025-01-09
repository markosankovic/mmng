#pragma once

#include <stdint.h>

#include "soem-master.h"

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
};
