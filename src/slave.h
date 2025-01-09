#pragma once

#include <nlohmann/json.hpp>

class Slave {
public:
  virtual nlohmann::json to_json() const = 0;

protected:
  Slave() {}
};
