#pragma once

#include "slave.h"

class Device {
protected:
  Slave &slave_;

public:
  explicit Device(Slave &slave) : slave_(slave) {}

  virtual ~Device() = default;
};
