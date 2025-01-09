#pragma once

#include "ethercat.h"

uint16_t get_ethercat_slave_state(ecx_contextt *context, uint16_t slave,
                                  bool force_refresh);
