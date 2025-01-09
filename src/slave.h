#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>

class Slave {
public:
  virtual nlohmann::json to_json() const = 0;

  virtual std::uint16_t get_state() = 0;

protected:
  Slave() {}
};
