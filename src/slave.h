#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>

#include "slave-info.h"

class Slave {
public:
  virtual uint16_t get_state() = 0;

  virtual SlaveInfo get_info() = 0;

protected:
  Slave() {}
};
