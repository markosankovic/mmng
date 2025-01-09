#include "soem.h"

uint16_t get_ethercat_slave_state(ecx_contextt *context, uint16_t slave,
                                  bool force_refresh) {
  if (force_refresh) {
    // The following code is based on the ecx_statecheck function from SOEM.
    // ecx_statecheck verifies whether the actual state of the slaves matches
    // the requested state. Note: ecx_statecheck might take at least 1 ms to
    // complete the check. Additionally, ecx_readstate reads the current state
    // from all connected slaves, so it should not be used to check the state of
    // a single slave.
    ec_alstatust slstat;
    slstat.alstatus = 0;
    slstat.alstatuscode = 0;
    ecx_FPRD(context->port, context->slavelist[slave].configadr, ECT_REG_ALSTAT,
             sizeof(slstat), &slstat, EC_TIMEOUTRET);
    uint16_t rval = etohs(slstat.alstatus);
    context->slavelist[slave].ALstatuscode = etohs(slstat.alstatuscode);
    context->slavelist[slave].state = rval;
  }

  return context->slavelist[slave].state;
}

std::string ethercat_slave_state_to_string(uint16_t state) {
  std::string str;
  if (state == EC_STATE_NONE) {
    str = "NONE";
  } else if (state == EC_STATE_INIT) {
    str = "INIT";
  } else if (state == (EC_STATE_INIT | EC_STATE_ERROR)) {
    str = "INIT + ERROR";
  } else if (state == EC_STATE_PRE_OP) {
    str = "PRE-OPERATIONAL";
  } else if (state == (EC_STATE_PRE_OP | EC_STATE_ERROR)) {
    str = "PRE-OPERATIONAL + ERROR";
  } else if (state == EC_STATE_BOOT) {
    str = "BOOT";
  } else if (state == EC_STATE_SAFE_OP) {
    str = "SAFE-OPERATIONAL";
  } else if (state == (EC_STATE_SAFE_OP | EC_STATE_ERROR)) {
    str = "SAFE-OPERATIONAL + ERROR";
  } else if (state == EC_STATE_OPERATIONAL) {
    str = "OPERATIONAL";
  } else if (state == (EC_STATE_OPERATIONAL | EC_STATE_ERROR)) {
    str = "OPERATIONAL + ERROR";
  } else {
    str = "UNRECOGNIZED";
  }
  str = "(" + std::to_string(state) + ") " + str;
  return str;
}

bool transition_to_higher_ethercat_slave_state(ecx_contextt *context,
                                               uint16_t slave,
                                               uint16_t target_state) {
  auto current_state = get_ethercat_slave_state(context, slave, true);

  if (current_state == EC_STATE_NONE) {
    return false;
  }

  bool has_error = (current_state & 0x0010) > 0;
  auto current_state_no_error = current_state & 0x000Fu;

  if (has_error) {
    context->slavelist[slave].state = current_state_no_error + EC_STATE_ACK;
    ecx_writestate(context, slave);
    ecx_statecheck(context, slave, current_state_no_error, EC_TIMEOUTSTATE);
    if (context->slavelist[slave].state != current_state_no_error) {
      return false;
    };
  }

  if (current_state_no_error == target_state) {
    return true;
  }

  if ((current_state_no_error > target_state) ||
      current_state_no_error == EC_STATE_BOOT ||
      target_state == EC_STATE_BOOT) {
    return false;
  }

  do {
    current_state_no_error *= 2;
    context->slavelist[slave].state = current_state_no_error;
    ecx_writestate(context, slave);
    ecx_statecheck(context, slave, current_state_no_error, EC_TIMEOUTSTATE);
    if (context->slavelist[slave].state != current_state_no_error) {
      return false;
    };
  } while (current_state_no_error != target_state);

  return true;
}

bool set_ethercat_slave_state(ecx_contextt *context, uint16_t slave,
                              uint16_t target_state) {
  auto current_state = get_ethercat_slave_state(context, slave, true);

  if (current_state == EC_STATE_NONE) {
    return false;
  }

  bool has_error = (current_state & 0x0010) > 0;
  auto current_state_no_error = current_state & 0x000Fu;

  if (has_error) {
    context->slavelist[slave].state = current_state_no_error + EC_STATE_ACK;
    ecx_writestate(context, slave);
    ecx_statecheck(context, slave, current_state_no_error, EC_TIMEOUTSTATE);
    if (context->slavelist[slave].state != current_state_no_error) {
      return false;
    };
  }

  // Return true if the current state is the same as the target state.
  if (current_state_no_error == target_state) {
    // Fix for Integro devices that incorrectly report INIT state instead of
    // INIT + ERROR.
    if (target_state == EC_STATE_INIT) {
      context->slavelist[slave].state = EC_STATE_INIT + EC_STATE_ACK;
      ecx_writestate(context, slave);
      ecx_statecheck(context, slave, EC_STATE_INIT, EC_TIMEOUTSTATE);
      if (context->slavelist[slave].state != EC_STATE_INIT) {
        return false;
      };
    }
    return true;
  }

  // If the current state is BOOT, transition to INIT first before moving to a
  // higher state.
  if (current_state_no_error == EC_STATE_BOOT) {
    update_mailbox_sync_managers_on_next_state(context, slave, EC_STATE_INIT);
    context->slavelist[slave].state = EC_STATE_INIT;
    ecx_writestate(context, slave);
    ecx_statecheck(context, slave, EC_STATE_INIT, EC_TIMEOUTSTATE);
    if (context->slavelist[slave].state != EC_STATE_INIT) {
      return false;
    };
    return transition_to_higher_ethercat_slave_state(context, slave,
                                                     target_state);
  }

  // If the target state is BOOT, first transition (drop) from a higher state to
  // INIT, then to BOOT.
  if (target_state == EC_STATE_BOOT) {
    if (current_state_no_error > EC_STATE_INIT) {
      context->slavelist[slave].state = EC_STATE_INIT;
      ecx_writestate(context, slave);
      ecx_statecheck(context, slave, EC_STATE_INIT, EC_TIMEOUTSTATE);
      if (context->slavelist[slave].state != EC_STATE_INIT) {
        return false;
      };
    }
    update_mailbox_sync_managers_on_next_state(context, slave, EC_STATE_BOOT);
    context->slavelist[slave].state = EC_STATE_BOOT;
    ecx_writestate(context, slave);
    ecx_statecheck(context, slave, EC_STATE_BOOT, EC_TIMEOUTSTATE);
    return context->slavelist[slave].state == EC_STATE_BOOT;
  }

  // Transitioning to a lower state from a higher state does not require
  // step-by-step transitions.
  if (current_state_no_error > target_state) {
    context->slavelist[slave].state = target_state;
    ecx_writestate(context, slave);
    ecx_statecheck(context, slave, target_state, EC_TIMEOUTSTATE);
    return context->slavelist[slave].state == target_state;
  }

  return transition_to_higher_ethercat_slave_state(context, slave,
                                                   target_state);
}

void update_mailbox_sync_managers_on_next_state(ecx_contextt *context,
                                                uint16_t slave,
                                                uint16_t target_state) {
  if (target_state == EC_STATE_BOOT) {
    /* read BOOT RX mailbox data, master -> slave */
    auto data = ec_readeeprom(slave, ECT_SII_BOOTRXMBX, EC_TIMEOUTEEP);
    context->slavelist[slave].SM[0].StartAddr = (uint16)LO_WORD(data);
    context->slavelist[slave].SM[0].SMlength = (uint16)HI_WORD(data);
    /* store boot write mailbox address */
    context->slavelist[slave].mbx_wo = (uint16)LO_WORD(data);
    /* store boot write mailbox size */
    context->slavelist[slave].mbx_l = (uint16)HI_WORD(data);

    /* read BOOT TX mailbox data, slave -> master */
    data = ec_readeeprom(slave, ECT_SII_BOOTTXMBX, EC_TIMEOUTEEP);
    context->slavelist[slave].SM[1].StartAddr = (uint16)LO_WORD(data);
    context->slavelist[slave].SM[1].SMlength = (uint16)HI_WORD(data);
    /* store boot read mailbox address */
    context->slavelist[slave].mbx_ro = (uint16)LO_WORD(data);
    /* store boot read mailbox size */
    context->slavelist[slave].mbx_rl = (uint16)HI_WORD(data);

    // LOG_F(INFO, "SM0 A:%4.4x L:%4d F:%8.8x",
    //       context->slavelist[slave].SM[0].StartAddr,
    //       ec_slave[slave].SM[0].SMlength,
    //       (int)context->slavelist[slave].SM[0].SMflags);
    // LOG_F(INFO, "SM1 A:%4.4x L:%4d F:%8.8x",
    //       context->slavelist[slave].SM[1].StartAddr,
    //       ec_slave[slave].SM[1].SMlength,
    //       (int)context->slavelist[slave].SM[1].SMflags);
    /* program SM0 mailbox in for slave */
    ec_FPWR(context->slavelist[slave].configadr, ECT_REG_SM0, sizeof(ec_smt),
            &context->slavelist[slave].SM[0], EC_TIMEOUTRET);
    /* program SM1 mailbox out for slave */
    ec_FPWR(context->slavelist[slave].configadr, ECT_REG_SM1, sizeof(ec_smt),
            &context->slavelist[slave].SM[1], EC_TIMEOUTRET);
  } else if (target_state == EC_STATE_INIT) {
    ecx_readeeprom1(context, slave, ECT_SII_RXMBXADR);
    uint32_t eedat = ecx_readeeprom2(context, slave, EC_TIMEOUTEEP);
    context->slavelist[slave].mbx_wo = (uint16)LO_WORD(etohl(eedat));
    context->slavelist[slave].mbx_l = (uint16)HI_WORD(etohl(eedat));
    context->slavelist[slave].SM[0].StartAddr =
        context->slavelist[slave].mbx_wo;
    context->slavelist[slave].SM[0].SMlength = context->slavelist[slave].mbx_l;

    ecx_readeeprom1(context, slave, ECT_SII_TXMBXADR);
    eedat = ecx_readeeprom2(context, slave, EC_TIMEOUTEEP);
    context->slavelist[slave].mbx_ro = (uint16)LO_WORD(etohl(eedat));
    context->slavelist[slave].mbx_rl = (uint16)HI_WORD(etohl(eedat));
    if (context->slavelist[slave].mbx_rl == 0) {
      context->slavelist[slave].mbx_rl = context->slavelist[slave].mbx_l;
    }
    context->slavelist[slave].SM[1].StartAddr =
        context->slavelist[slave].mbx_ro;
    context->slavelist[slave].SM[1].SMlength = context->slavelist[slave].mbx_rl;

    // LOG_F(INFO, "SM0 A:%4.4x L:%4d F:%8.8x",
    //       context->slavelist[slave].SM[0].StartAddr,
    //       context->slavelist[slave].SM[0].SMlength,
    //       (int)context->slavelist[slave].SM[0].SMflags);
    // LOG_F(INFO, "SM1 A:%4.4x L:%4d F:%8.8x",
    //       context->slavelist[slave].SM[1].StartAddr,
    //       context->slavelist[slave].SM[1].SMlength,
    //       (int)context->slavelist[slave].SM[1].SMflags);
    /* program SM0 mailbox in for slave */
    ec_FPWR(context->slavelist[slave].configadr, ECT_REG_SM0, sizeof(ec_smt),
            &context->slavelist[slave].SM[0], EC_TIMEOUTRET);
    /* program SM1 mailbox out for slave */
    ec_FPWR(context->slavelist[slave].configadr, ECT_REG_SM1, sizeof(ec_smt),
            &context->slavelist[slave].SM[1], EC_TIMEOUTRET);
  }
}
