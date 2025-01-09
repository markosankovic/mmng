#pragma once

#include "master.h"

#include <loguru.h>

class SoemMaster : public Master {
public:
  void init(const char *ifname) override {
    // This prevents ecx_config_map_group from transitioning devices without
    // firmware installed into SAFE-OPERATIONAL state.
    // SOMANET devices without firmware that are transitioned into this state
    // become stuck and can only be recovered by power cycling the device.
    ecx_context.manualstatechange = TRUE;

    if (ec_init(ifname)) {
      LOG_F(INFO, "ec_init on %s succeeded.\n", ifname);
      if (ec_config_init(FALSE) > 0) {
        LOG_F(INFO, "%d slaves found and configured.\n", ec_slavecount);

        for (int i = 1; i <= ec_slavecount; i++) {
          auto slave = ec_slave[i];
          LOG_F(INFO, "Slave %d: Name: %s", i, slave.name);
        }
      }
    }
  };
};
