#pragma once

#include "ethercat.h"
#include <cstdint>
#include <string>

/**
 * Retrieves the current state of a specific EtherCAT slave. If `force_refresh`
 * is true, the state is explicitly read from the slave hardware.
 *
 * @param context A pointer to the EtherCAT master context.
 * @param slave The position of the target slave.
 * @param force_refresh If true, explicitly reads the state from the slave
 * hardware.
 * @return The current state of the slave (e.g., EC_STATE_INIT,
 * EC_STATE_OPERATIONAL).
 */
uint16_t get_ethercat_slave_state(ecx_contextt *context, uint16_t slave,
                                  bool force_refresh);

/**
 * Converts a numeric EtherCAT state value into a human-readable string.
 *
 * @state The numeric EtherCAT state value.
 */
std::string ethercat_slave_state_to_string(uint16_t state);

/**
 * Transitions an EtherCAT slave to a higher state step-by-step, ensuring
 * all intermediate states are passed during the process.
 *
 * @param context A pointer to the EtherCAT master context.
 * @param slave The position of the target slave.
 * @param target_state The desired EtherCAT state to reach.
 * @return True if the slave successfully transitions to the target state, false
 * otherwise.
 */
bool transition_to_higher_ethercat_slave_state(ecx_contextt *context,
                                               uint16_t slave,
                                               uint16_t target_state);

/**
 * Sets the EtherCAT slave to a specified state while managing:
 * - Error handling, including transitioning out of error states.
 * - Special scenarios, such as transitions to and from the BOOT state.
 * - Automatic step-by-step transitions for progressing to higher states.
 *
 * @param context A pointer to the EtherCAT master context.
 * @param slave The position of the slave to transition.
 * @param target_state The desired EtherCAT state to reach.
 * @return True if the slave successfully transitions to the target state, false
 * otherwise.
 */
bool set_ethercat_slave_state(ecx_contextt *context, uint16_t slave,
                              uint16_t target_state);

/**
 * Updates the mailbox and sync manager (SM) configurations for an EtherCAT
 * slave when transitioning to specific states (e.g., BOOT or INIT).
 *
 * - For the BOOT state, the function reads the BOOT RX and TX mailbox
 * information and programs the corresponding SM configurations.
 * - For the INIT state, it reads the RX and TX mailbox addresses and lengths,
 *   and configures the respective sync managers.
 *
 * @param context Pointer to the EtherCAT master context.
 * @param slave The position of the target slave.
 * @param target_state The target EtherCAT state transitioning to (e.g.,
 * EC_STATE_BOOT, EC_STATE_INIT).
 */
void update_mailbox_sync_managers_on_next_state(ecx_contextt *context,
                                                uint16_t slave,
                                                uint16_t target_state);
