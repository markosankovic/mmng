#pragma once

#include <memory>
#include <vector>

#include "slave.h"

class Master {
protected:
  Master() {}

public:
  std::vector<std::unique_ptr<Slave>> slaves_;

  virtual ~Master() {}

  virtual void init(const std::string ifname) = 0;

  virtual void deinit() = 0;
};
