#pragma once

#include <loguru.h>

#include "master.h"
#include "soem_slave.h"

struct Fieldbus {
  ecx_contextt context;
  std::string iface;
  uint8 group;
  int roundtrip_time;

  /* Used by the context */
  uint8 map[4096];
  ecx_portt port;
  ec_slavet slavelist[EC_MAXSLAVE];
  int slavecount;
  ec_groupt grouplist[EC_MAXGROUP];
  uint8 esibuf[EC_MAXEEPBUF];
  uint32 esimap[EC_MAXEEPBITMAP];
  ec_eringt elist;
  ec_idxstackT idxstack;
  boolean ecaterror;
  int64 DCtime;
  ec_SMcommtypet SMcommtype[EC_MAX_MAPT];
  ec_PDOassignt PDOassign[EC_MAX_MAPT];
  ec_PDOdesct PDOdesc[EC_MAX_MAPT];
  ec_eepromSMt eepSM;
  ec_eepromFMMUt eepFMMU;
};

static void fieldbus_initialize(std::unique_ptr<Fieldbus> &fieldbus,
                                const std::string &iface) {
  fieldbus = std::make_unique<Fieldbus>();

  memset(fieldbus.get(), 0, sizeof(Fieldbus));

  fieldbus->iface = iface;
  fieldbus->group = 0;
  fieldbus->roundtrip_time = 0;
  fieldbus->ecaterror = FALSE;

  ecx_contextt *context = &fieldbus->context;
  context->port = &fieldbus->port;
  context->slavelist = fieldbus->slavelist;
  context->slavecount = &fieldbus->slavecount;
  context->maxslave = EC_MAXSLAVE;
  context->grouplist = fieldbus->grouplist;
  context->maxgroup = EC_MAXGROUP;
  context->esibuf = fieldbus->esibuf;
  context->esimap = fieldbus->esimap;
  context->esislave = 0;
  context->elist = &fieldbus->elist;
  context->idxstack = &fieldbus->idxstack;
  context->ecaterror = &fieldbus->ecaterror;
  context->DCtime = &fieldbus->DCtime;
  context->SMcommtype = fieldbus->SMcommtype;
  context->PDOassign = fieldbus->PDOassign;
  context->PDOdesc = fieldbus->PDOdesc;
  context->eepSM = &fieldbus->eepSM;
  context->eepFMMU = &fieldbus->eepFMMU;
  context->FOEhook = NULL;
  context->EOEhook = NULL;
  context->manualstatechange = 0;
}

class SoemMaster : public Master {

  std::unique_ptr<Fieldbus> fieldbus_;

public:
  ~SoemMaster() override { deinit(); }

  void init(const std::string ifname) override {
    if (fieldbus_) {
      throw std::runtime_error("EtherCAT master has already been initialized.");
    }

    fieldbus_initialize(fieldbus_, ifname);

    // This prevents ecx_config_map_group from transitioning devices without
    // firmware installed into SAFE-OPERATIONAL state.
    // SOMANET devices without firmware that are transitioned into this state
    // become stuck and can only be recovered by power cycling the device.
    fieldbus_->context.manualstatechange = TRUE;

    auto ecx_init_result = ecx_init(&fieldbus_->context, ifname.c_str());

    if (ecx_init_result <= 0) {
      throw std::runtime_error("ecx_init failed: " +
                               std::to_string(ecx_init_result));
    }

    LOG_F(INFO, "ecx_init on %s succeeded.", ifname.c_str());

    auto ecx_config_init_result = ecx_config_init(&fieldbus_->context, FALSE);

    if (ecx_config_init_result <= 0) {
      throw std::runtime_error("ecx_config_init failed: " +
                               std::to_string(ecx_config_init_result));
    }

    LOG_F(INFO, "%d slaves found and configured.",
          *fieldbus_->context.slavecount);

    for (uint8_t i = 1; i <= *fieldbus_->context.slavecount; i++) {
      auto slave = std::make_unique<SoemSlave>(&fieldbus_->context, i);
      slaves_.push_back(std::move(slave));
    }

    LOG_F(INFO, "EtherCAT master has been initialized.");
  };

  void deinit() override {
    slaves_.clear();
    ecx_close(&fieldbus_->context);
    fieldbus_.reset();
    LOG_F(INFO, "EtherCAT master has been deinitialized.");
  }
};
