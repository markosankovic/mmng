#pragma once

#include <loguru.h>

#include "master.h"
#include "soem_slave.h"

class SoemMaster : public Master {

  std::unique_ptr<ecx_contextt> context_;

public:
  ~SoemMaster() override { deinit(); }

  void init(const char *ifname) override {
    context_ = std::make_unique<ecx_contextt>(ecx_context);
    // This prevents ecx_config_map_group from transitioning devices without
    // firmware installed into SAFE-OPERATIONAL state.
    // SOMANET devices without firmware that are transitioned into this state
    // become stuck and can only be recovered by power cycling the device.
    context_->manualstatechange = TRUE;

    auto ecx_init_result = ecx_init(context_.get(), ifname);

    if (ecx_init_result <= 0) {
      LOG_F(INFO, "ecx_init on %s failed.", ifname);
      throw std::runtime_error("ecx_init failed: " +
                               std::to_string(ecx_init_result));
    }

    LOG_F(INFO, "ecx_init on %s succeeded.", ifname);

    auto ecx_config_init_result = ecx_config_init(context_.get(), FALSE);

    if (ecx_config_init_result <= 0) {
      LOG_F(INFO, "ecx_config_init failed.");
      throw std::runtime_error("ecx_config_init failed: " +
                               std::to_string(ecx_config_init_result));
    }

    LOG_F(INFO, "%d slaves found and configured.", *context_->slavecount);

    for (uint8_t i = 1; i <= *context_->slavecount; i++) {
      auto slave = std::make_unique<SoemSlave>(context_, i);
      slaves_.push_back(std::move(slave));
    }

    LOG_F(INFO, "EtherCAT master has been initialized.");
  };

  void deinit() override {
    slaves_.clear();
    if (context_) {
      ecx_close(context_.get());
    }
    LOG_F(INFO, "EtherCAT master has been deinitialized.");
  }
};
