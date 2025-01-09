#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

struct SlaveInfo {
  uint16_t position;
  std::string name;
  uint16_t state;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SlaveInfo, position, name, state)
