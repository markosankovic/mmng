#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>

#include "parameter.h"

struct SlaveInfo {
  uint16_t position;
  std::string name;
  uint16_t state;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SlaveInfo, position, name, state)

class Slave {
public:
  virtual uint16_t get_state() = 0;

  virtual SlaveInfo get_info() = 0;

  virtual bool set_state(uint16_t target_state) = 0;

  virtual int upload(uint16_t index, uint8_t subindex) = 0;

  virtual void loadParameters() = 0;

protected:
  Slave() {}

  std::map<std::pair<uint16_t, uint8_t>, Parameter> parameters;
};
