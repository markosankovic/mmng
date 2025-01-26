#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <vector>

#include "parameter.h"

struct SlaveInfo {
  uint16_t position;
  std::string name;
  uint16_t state;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SlaveInfo, position, name, state)

class Slave {
protected:
  Slave() {}

  std::map<std::pair<uint16_t, uint8_t>, Parameter> parametersMap_;

public:
  virtual uint16_t get_state() = 0;

  virtual SlaveInfo get_info() = 0;

  virtual bool set_state(uint16_t target_state) = 0;

  virtual ValueType upload(uint16_t index, uint8_t subindex,
                           bool refresh = true) = 0;

  virtual void loadParameters() = 0;

  virtual void clearParameters() = 0;

  virtual std::vector<Parameter> getParameters() = 0;
};
