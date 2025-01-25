#pragma once

#include <loguru.h>

#include "master.h"
#include "soem_slave.h"

class SoemMaster : public Master {

  std::unique_ptr<ecx_contextt> context;

public:
  ~SoemMaster() override { deinit(); }

  void init(const char *ifname) override {
    context = std::make_unique<ecx_contextt>(ecx_context);
    // This prevents ecx_config_map_group from transitioning devices without
    // firmware installed into SAFE-OPERATIONAL state.
    // SOMANET devices without firmware that are transitioned into this state
    // become stuck and can only be recovered by power cycling the device.
    context->manualstatechange = TRUE;

    if (ecx_init(context.get(), ifname)) {
      LOG_F(INFO, "ec_init on %s succeeded.", ifname);
      if (ecx_config_init(context.get(), FALSE) > 0) {
        LOG_F(INFO, "%d slaves found and configured.", *context->slavecount);
        for (uint8_t i = 1; i <= *context->slavecount; i++) {
          auto slave = std::make_unique<SoemSlave>(context, i);
          slaves.push_back(std::move(slave));
        }
        LOG_F(INFO, "EtherCAT master has been initialized.");
      }
    }
  };

  void deinit() override {
    slaves.clear();
    if (context) {
      ecx_close(context.get());
    }
    LOG_F(INFO, "EtherCAT master has been deinitialized.");
  }
};
