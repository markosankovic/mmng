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

  virtual void init(const char *ifname) = 0;

  virtual void deinit() = 0;
};
