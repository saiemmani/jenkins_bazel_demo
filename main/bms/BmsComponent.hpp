/*
Copyright (c) 2019, BMW Group. All rights reserved.
BMW Group and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from BMW Group is strictly prohibited.
*/

#pragma once

#include <future>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "CanRecvHandler.hpp"
#include "can-lib/CanNetwork.hpp"
#include "components/bms/battery_state.capnp.h"
#include "engine/alice/alice_codelet.hpp"
#include "engine/alice/components/Failsafe.hpp"
#include "engine/gems/state_machine/state_machine.hpp"
#include "messages/STRmessages.hpp"
#include "messages/messages.hpp"
#include "packages/engine_gems/state/io.hpp"
#include "str-hw/EvaBMS.hpp"

/** How many resets should the bms try before entering in Error state */
#define RESET_RETRIES 1
/** How much time we should wait between a reset signal and close MSW signal */
#define RESET_WAIT_TIME_S 2.5
/** How much time we should wait for the switch to close before timing out */
#define AFTER_RESET_WAIT_TIME_S 5.0

using CompErrSev = ComponentErrorProto::Severity;

namespace str {

const std::map<std::string, ComponentError> bms_error_map = {
    // {MKNXVEHCL0042M,
    //  {"MKNXVEHCL0042M", "BMS: Charging Active", "Robot is in Charging."}},
    {"MKNXVEHCL1500M",
     {"MKNXVEHCL1500M", "Closing the Main Switch is not possible", "Contact Expert",
      CompErrSev::FATAL}},
    // {MKNXVEHCL1501M,
    //  {"MKNXVEHCL1501M", "BMS: Timeout Error (No stay-alive bit)", "Contact Expert"}},
    // {MKNXVEHCL1508M,
    //  {"MKNXVEHCL1508M", "BMS: Cell under-voltage error limit reached",
    //   "Contact Expert"}},
    // {MKNXVEHCL1514M,
    //  {"MKNXVEHCL1514M", "BMS: Start-up test error occurred", "Contact Expert"}},
    // {MKNXVEHCL1516M,
    //  {"MKNXVEHCL1516M", "BMS: Main Switch stuck detected", "Contact Expert"}},
    // {TKNXVEHCL0041T,
    // {"TKNXVEHCL0041T", "BMS: Cell Under-Voltage Error",
    //   "Inspect the battery. Charge the robot. Do not drive it. Contact an expert."}},
    {"MIDLBMSER0000M",
     {"MIDLBMSER0000M", "CAN interface not found in PC",
      "Check the PEAK connection if any. Contact expert if no PEAK in the robot",
      CompErrSev::FATAL}},
    {"MIDLBMSER0001M",
     {"MIDLBMSER0001M", "An error occured resetting the CAN interface", "Contact expert",
      CompErrSev::RECOVERABLE}},
    {"MIDLBMSER0002M",
     {"MIDLBMSER0002M", "BMS not detected on CAN network",
      "Check the cable going from the BMS to the PC", CompErrSev::FATAL}},
    {"MIDLBMSER0004M",
     {"MIDLBMSER0004M", "BMS switch switched off for no reason", "Contact expert",
      CompErrSev::RECOVERABLE}},
};

class Bms : public isaac::alice::Codelet {
 public:
  void initialize() override;
  void start() override;
  void tick() override;
  void stop() override;

  // Name of the can interface the BMS is connected to
  ISAAC_PARAM(std::string, if_name, "can0")
  // Minimum battery voltage before sending a discharge warning
  ISAAC_PARAM(int, minBatteryVoltage, 40)
  // Minimum State of charge before sending a discharge warning
  ISAAC_PARAM(int, minSOC, 40)
  // Minimum Minimum cell voltage before sending a discharge warning
  ISAAC_PARAM(double, minCellVoltage, 3.5)
  // Force request shutdown of the main switch of the BMS
  ISAAC_PARAM(bool, request_shutdown, false)
  // Number of the core to run the main thread on, -1 for not specified
  ISAAC_PARAM(int, core_idx, -1)

  // State of the STR Battery containing all battery information
  ISAAC_PROTO_TX(BatteryStateProto, battery_state)
  ISAAC_PROTO_TX(ComponentStateProto, state)

  // Force shutdown the BMS
  ISAAC_PROTO_RX(BoolProto, force_shutdown)

  std::unique_ptr<CanInterface> can_if;
  std::unique_ptr<Eva_BMS> my_bms;
  std::promise<void> running;
  std::unique_ptr<std::thread> op_thread;

  bool isError;
  bool isSuccess;
  bool isInitialized;
  ComponentError compError;
  double lastTickTime;

  void sendBatteryStateProto();

 private:
  CanRecvHandler* recv_handler;
  using State = std::string;
  isaac::state_machine::StateMachine<State> machine_;

  std::string errorMessages;

  int resetCounter; /**< Counter for how many tries are left when resetting the bms*/
  double
      transitionTickTime; /**< Variable holding the moment the state machine enters in init state*/

  isaac::Uuid last_request_uuid;

  void createStateMachine();
  void updateReceivedChannels();
};

}  // namespace str

ISAAC_ALICE_REGISTER_CODELET(str::Bms);