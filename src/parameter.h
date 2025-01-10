#pragma once

#include <cstdint>
#include <string>

struct Parameter {
  std::string name;
  std::uint16_t index;
  std::uint8_t subindex;
  std::uint8_t data[];
};
