#pragma once

#include <memory>
#include <vector>

#include "slave.h"

class Master {
public:
  std::vector<std::unique_ptr<Slave>> slaves;

  virtual ~Master() {}

  virtual void init(const char *ifname) = 0;

protected:
  Master() {}
};
