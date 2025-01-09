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
