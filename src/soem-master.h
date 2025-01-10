#pragma once

#include <loguru.h>

#include "master.h"
#include "soem-slave.h"

class SoemMaster : public Master {
public:
  ~SoemMaster() override { deinit(); }

  void init(const char *ifname) override {
    // This prevents ecx_config_map_group from transitioning devices without
    // firmware installed into SAFE-OPERATIONAL state.
    // SOMANET devices without firmware that are transitioned into this state
    // become stuck and can only be recovered by power cycling the device.
    ecx_context.manualstatechange = TRUE;

    if (ec_init(ifname)) {
      LOG_F(INFO, "ec_init on %s succeeded.", ifname);
      if (ec_config_init(FALSE) > 0) {
        LOG_F(INFO, "%d slaves found and configured.", ec_slavecount);
        for (uint8_t i = 1; i <= ec_slavecount; i++) {
          auto slave = std::make_unique<SoemSlave>(i);
          slaves.push_back(std::move(slave));
        }
        LOG_F(INFO, "EtherCAT master has been initialized.");
      }
    }
  };

  void deinit() override {
    slaves.clear();
    ec_close();
    LOG_F(INFO, "EtherCAT master has been deinitialized.");
  }
};
