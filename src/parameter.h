#pragma once

#include "ethercat.h"
#include <cstdint>
#include <string>
#include <variant>

using ValueType =
    std::variant<bool, std::int8_t, std::int16_t, std::int32_t, std::int64_t,
                 std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t,
                 float, double, std::string>;

struct Parameter {
  std::string name;
  std::uint16_t index;
  std::uint8_t subindex;
  std::uint16_t bitLength;
  int byteLength;
  std::uint16_t dataType;
  std::uint8_t objectCode;
  std::uint16_t objAccess;
  std::shared_ptr<std::uint8_t[]> data;

  ValueType getValue() const {
    switch (dataType) {
    case ec_datatype::ECT_BOOLEAN:
      return *reinterpret_cast<const bool *>(data.get());
    case ec_datatype::ECT_INTEGER8:
      return *reinterpret_cast<const std::int8_t *>(data.get());
    case ec_datatype::ECT_INTEGER16:
      return *reinterpret_cast<const std::int16_t *>(data.get());
    case ec_datatype::ECT_INTEGER24:
    case ec_datatype::ECT_INTEGER32:
      return *reinterpret_cast<const std::int32_t *>(data.get());
    case ec_datatype::ECT_INTEGER64:
      return *reinterpret_cast<const std::int64_t *>(data.get());
    case ec_datatype::ECT_UNSIGNED8:
      return *reinterpret_cast<const std::uint8_t *>(data.get());
    case ec_datatype::ECT_UNSIGNED16:
      return *reinterpret_cast<const std::uint16_t *>(data.get());
    case ec_datatype::ECT_UNSIGNED24:
    case ec_datatype::ECT_UNSIGNED32:
      return *reinterpret_cast<const std::uint32_t *>(data.get());
    case ec_datatype::ECT_UNSIGNED64:
      return *reinterpret_cast<const std::uint64_t *>(data.get());
    case ec_datatype::ECT_REAL32:
      return *reinterpret_cast<const float *>(data.get());
    case ec_datatype::ECT_REAL64:
      return *reinterpret_cast<const double *>(data.get());
    case ec_datatype::ECT_VISIBLE_STRING:
    case ec_datatype::ECT_OCTET_STRING:
    case ec_datatype::ECT_UNICODE_STRING:
      return std::string(reinterpret_cast<const char *>(data.get()));
    default:
      throw std::runtime_error("Unsupported data type. " +
                               std::to_string(dataType));
    }
  }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Parameter, name, index, bitLength,
                                   byteLength, dataType, objAccess, subindex,
                                   objectCode)
