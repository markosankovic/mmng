#pragma once

#include <cstdint>
#include <string>

struct Parameter {
  std::string name;
  std::uint16_t index;
  std::uint16_t bitLength;
  std::uint16_t dataType;
  std::uint16_t objAccess;
  std::uint8_t subindex;
  std::uint8_t objectCode;
  std::uint8_t *data;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Parameter, name, index, bitLength, dataType,
                                   objAccess, subindex, objectCode)
