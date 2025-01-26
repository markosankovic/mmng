#pragma once

#include <cstdint>
#include <mutex>

#include "soem_utils.h"

class SoemSlave : public Slave {

  std::unique_ptr<ecx_contextt> &context_;

  uint8_t position_;

  std::mutex mailboxMutex_;

public:
  SoemSlave(std::unique_ptr<ecx_contextt> &context, const uint8_t position)
      : context_(context), position_(position) {}

  uint16_t get_state() override {
    return get_ethercat_slave_state(context_.get(), position_, true);
  }

  SlaveInfo get_info() override {
    SlaveInfo info;
    info.position = position_;
    info.name = context_->slavelist[position_].name;
    info.state = get_state();
    return info;
  }

  bool set_state(uint16_t target_state) override {
    return set_ethercat_slave_state(context_.get(), position_, target_state);
  }

  void loadParameters() override {
    std::lock_guard<std::mutex> lock(mailboxMutex_);

    if (parametersMap_.size() > 0) {
      throw std::runtime_error("Parameters already loaded!");
    }

    auto state = get_state();
    if (state != EC_STATE_PRE_OP) {
      throw std::runtime_error(
          "To set up object dictionary entries, the device "
          "must be in the (2) PRE-OPERATIONAL state. The "
          "current state is " +
          ethercat_slave_state_to_string(state) + ".");
    }

    ec_ODlistt od_list;

    memset(&od_list, 0, sizeof(od_list));
    int error = ecx_readODlist(context_.get(), position_, &od_list);

    if (error <= 0) {
      throw std::runtime_error(
          "Failed to read the object dictionary list using "
          "ecx_readODlist! Error code: " +
          std::to_string(error));
    }

    LOG_F(INFO, "Device %d: Read %u dictionary objects. Configuring...",
          position_, od_list.Entries);

    ec_OElistt oe_list;

    for (uint16_t i = 0; i < od_list.Entries; i++) {
      error = ecx_readODdescription(context_.get(), i, &od_list);

      if (error <= 0) {
        LOG_F(WARNING,
              "Device %d: Failed to read the object dictionary description for "
              "%#04x!",
              position_, od_list.Index[i]);
      }

      memset(&oe_list, 0, sizeof(oe_list));
      int error = ecx_readOE(context_.get(), i, &od_list, &oe_list);

      if (error <= 0) {
        LOG_F(ERROR,
              "Device %d: Failed to read the object dictionary entries for "
              "%#04x!",
              position_, od_list.Index[i]);
        throw std::runtime_error(
            "Failed to read the object dictionary entries!");
      }

      for (uint8_t j = 0; j <= od_list.MaxSub[i]; j++) {
        auto [iterator, success] = parametersMap_.try_emplace(
            std::pair{od_list.Index[i], j}, Parameter{});

        if (success) {
          auto &parameter = iterator->second;
          parameter.name = od_list.Name[i];

          parameter.index = od_list.Index[i];
          parameter.subindex = j;

          parameter.dataType = oe_list.DataType[j];

          parameter.bitLength = oe_list.BitLength[j];
          auto byteLength = 1;
          if (parameter.bitLength > 8) {
            byteLength = parameter.bitLength % 8 == 0
                             ? parameter.bitLength / 8
                             : parameter.bitLength / 8 + 1;
          }
          parameter.byteLength = byteLength;

          parameter.objectCode = od_list.ObjectCode[i];

          parameter.objAccess = oe_list.ObjAccess[j];

          parameter.data = std::make_shared<std::uint8_t[]>(byteLength);
        } else {
          LOG_F(ERROR,
                "Device %d: Failed to add %#04x:%02x to the list of object "
                " dictionary entries !",
                position_, od_list.Index[i], j);
        }
      }
    }

    LOG_F(INFO, "Device %d: Configured %lu object dictionary entries.",
          position_, parametersMap_.size());

    return;
  }

  void clearParameters() override { parametersMap_.clear(); }

  std::vector<Parameter> getParameters() override {
    std::vector<Parameter> parameters;
    for (const auto &[key, value] : parametersMap_) {
      parameters.push_back(value);
    }
    return parameters;
  }

  ValueType upload(uint16_t index, uint8_t subindex) override {
    std::lock_guard<std::mutex> lock(mailboxMutex_);

    auto state = get_state();
    if (state == EC_STATE_INIT || state == EC_STATE_BOOT) {
      throw std::runtime_error(
          "To upload object dictionary entries, the device must be in the (2) "
          "PRE-OPERATIONAL, (4) SAFE-OPERATIONAL, or (8) OPERATIONAL state. "
          "The current state is " +
          ethercat_slave_state_to_string(state) + ".");
    }

    auto iterator = parametersMap_.find(std::pair{index, subindex});
    if (iterator == parametersMap_.end()) {
      throw std::out_of_range(
          "Object dictionary entry not found! Index: " + std::to_string(index) +
          ", Subindex: " + std::to_string(subindex));
    }

    auto &parameter = iterator->second;

    ecx_SDOread(context_.get(), position_, parameter.index, parameter.subindex,
                false, &parameter.byteLength, parameter.data.get(),
                EC_TIMEOUTRXM * 3);

    return parameter.getValue();
  }
};
