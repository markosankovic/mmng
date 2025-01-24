#include <iostream>
#include <loguru.h>
#include <soem_utils.h>

char IOmap[4096];
int expectedWKC;
const char *ifname = "enx1c1adff64fae";

int main() {
  LOG_F(INFO, "Starting rach_preop");

  if (ec_init(ifname)) {
    LOG_F(INFO, "ec_init on %s succeeded.", ifname);

    if (ec_config_init(FALSE) > 0) {
      LOG_F(INFO, "%d slaves found and configured.", ec_slavecount);
      LOG_F(INFO, "Slave state %s",
            ethercat_slave_state_to_string(
                get_ethercat_slave_state(&ecx_context, 1, true))
                .c_str());

      ec_config_map(&IOmap);
      LOG_F(INFO, "Slaves mapped, state to SAFE_OP.");
      ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);

      expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
      LOG_F(INFO, "Calculated workcounter %d", expectedWKC);

      ec_slave[0].state = EC_STATE_OPERATIONAL;
      ec_send_processdata();
      ec_receive_processdata(EC_TIMEOUTRET);
      ec_writestate(0);
      int chk = 200;
      /* wait for all slaves to reach OP state */
      do {
        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);
        ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
      } while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

      if (ec_slave[0].state == EC_STATE_OPERATIONAL) {
        LOG_F(INFO, "Operational state reached for all slaves.");

        int i, j, oloop, iloop, wkc;

        for (i = 1; i <= 1000; i++) {
          ec_send_processdata();
          wkc = ec_receive_processdata(EC_TIMEOUTRET);
          osal_usleep(1000);
        }
      }
    } else {
      LOG_F(ERROR, "Not all slaves reached operational state.");
      ec_readstate();
      for (int i = 1; i <= ec_slavecount; i++) {
        if (ec_slave[i].state != EC_STATE_OPERATIONAL) {
          LOG_F(ERROR, "Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s", i,
                ec_slave[i].state, ec_slave[i].ALstatuscode,
                ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
        }
      }
    }
  }

  return 0;
}
