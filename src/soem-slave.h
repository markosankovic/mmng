#pragma once

#include <cstdint>

#include "soem.h"

class SoemSlave : public Slave {

  uint8_t position;

public:
  SoemSlave(const uint8_t position) : position(position) {}

  uint16_t get_state() override {
    return get_ethercat_slave_state(&ecx_context, position, true);
  }

  SlaveInfo get_info() override {
    SlaveInfo info;
    info.position = position;
    info.name = ec_slave[position].name;
    info.state = get_state();
    return info;
  }

  bool set_state(uint16_t target_state) override {
    return set_ethercat_slave_state(&ecx_context, position, target_state);
  }

  void loadParameters() override {
    ec_ODlistt od_list;
    memset(&od_list, 0, sizeof(od_list));

    int error = ecx_readODlist(&ecx_context, position, &od_list);
    if (error <= 0) {
      LOG_F(ERROR, "Device %d: Failed to read the object dictionary list!",
            position);
      throw std::runtime_error(
          "Failed to read the object dictionary list using ecx_readODlist!");
    }

    return;
  }

  int upload(uint16_t index, uint8_t subindex) override { return 123; }
};
