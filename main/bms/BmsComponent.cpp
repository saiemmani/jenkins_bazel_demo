/*
Copyright (c) 2019, BMW Group. All rights reserved.
BMW Group and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from BMW Group is strictly prohibited.
*/

#include "BmsComponent.hpp"

namespace str {

constexpr char kStateInit[] = "kInitBMS";
constexpr char kStateCheckConnection[] = "kCheckConnectionBMS";
constexpr char kStateResetInterface[] = "kResetInterface";
constexpr char kStateReset[] = "kResetBMS";
constexpr char kStateWaitForReset[] = "kWaitForResetBMS";
constexpr char kStateOperational[] = "kOperationalBMS";
constexpr char kStateError[] = "kErrorBMS";

using namespace std;

void op_main(Bms* p, std::future<void> ft) {
  while (ft.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
    double dt = getTimeOfDayDouble() - p->lastTickTime;
    if (dt > 0.08) {
      LOG_ERROR("Time between executions > 80ms: %2.4f", dt);
    }

    if (!p->get_request_shutdown()) {
      p->sendBatteryStateProto();
      p->my_bms->SendSynchronizationFrame(true);
    }

    p->lastTickTime = getTimeOfDayDouble();

    usleep(20 * 1000);
  }
}

void Bms::initialize() {
  // Create the network and BMS objects
  this->can_if = make_unique<CanInterface>();
  this->my_bms = make_unique<Eva_BMS>(this->can_if.get());

  // Create state machine
  this->createStateMachine();
}

void Bms::start() {
  this->isInitialized = false;
  this->isSuccess = false;
  this->isError = false;
  this->resetCounter = RESET_RETRIES;

  this->recv_handler = this->node()->getComponentOrNull<CanRecvHandler>();
  if (!this->recv_handler)
    this->recv_handler = this->node()->addComponent<CanRecvHandler>("can_recv_handler");
  this->recv_handler->can_if = this->can_if.get();

  // publish state init
  auto state_pr = tx_state().initProto();
  state_pr.setState(ComponentStateProto::State::INIT);
  tx_state().publish();

  // Create the state machine and tick periodically
  this->machine_.start(kStateInit);
  tickPeriodically();
}

void Bms::tick() {
  // Update received data
  updateReceivedChannels();
  machine_.tick();
}

void Bms::stop() {
  // publish state stopped
  auto state_pr = tx_state().initProto();
  state_pr.setState(ComponentStateProto::State::STOPPED);
  tx_state().publish();

  // this->my_bms.get()->SendCompletionFrames();
  // this->my_bms.get()->sendResetFrame();
  this->recv_handler->stop();
  this->can_if->disconnect();
  this->machine_.stop();
  this->errorMessages.clear();
}

void Bms::updateReceivedChannels() {
  if (rx_force_shutdown().available()) {
    set_request_shutdown(rx_force_shutdown().getProto().getVal());
  }
}

void Bms::createStateMachine() {
  machine_.setToString([&](const std::string& state) { return state; });

  machine_.addState(
      kStateInit,
      [this] {
        // publish state init
        auto state_pr = tx_state().initProto();
        state_pr.setState(ComponentStateProto::State::INIT);
        tx_state().publish();

        // Create the network and bms
        std::string ec;
        int res = this->can_if->connect(get_if_name(), ec, false);
        if (res < 0) {
          this->compError = bms_error_map.at("MIDLBMSER0000M");
          this->compError.description = "Error connecting to " + get_if_name() + ", " + ec;
          this->isError = true;
          return;
        }
        int res2 = this->can_if->resetInterface(ec);
        if (res2 < 0) {
          this->compError = bms_error_map.at("MIDLBMSER0001M");
          this->compError.description = ", if_name: " + get_if_name() + ", " + ec;
          this->isError = true;
          return;
        }

        // Create the reception conponent
        this->recv_handler->async_set_tick_period("0.1ms");
        this->recv_handler->start();

        this->isInitialized = true;
        async_set_request_shutdown(false);
      },
      [this] {}, [] {});

  machine_.addState(
      kStateCheckConnection, [this] { this->transitionTickTime = this->getTickTime(); }, [this] {},
      [] {});

  machine_.addState(
      kStateResetInterface,
      [this] {
        this->isSuccess = false;
        this->isError = false;
      },
      [this] {
        // reset interface
        std::string err_msg;
        int res = this->can_if->resetInterface(err_msg);
        if (res < 0) {
          this->compError = bms_error_map.at("MIDLBMSER0001M");
          this->compError.description += ", " + err_msg;
          this->isError = true;
        } else {
          this->isSuccess = true;
        }
      },
      [] {});

  machine_.addState(
      kStateReset,
      [this] {
        this->my_bms->sendResetFrame();
        this->transitionTickTime = this->getTickTime();
      },
      [this] {}, [] {});

  machine_.addState(
      kStateWaitForReset,
      [this] {
        this->transitionTickTime = this->getTickTime();
        this->my_bms->SendSynchronizationFrame();
      },
      [this] { this->my_bms->SendSynchronizationFrame(); }, [] {});

  machine_.addState(
      kStateError,
      [this] {
        // log error
        LOG_ERROR("Error: %s, %s", this->compError.code.c_str(),
                  this->compError.description.c_str());

        // publish state error
        auto state_pr = tx_state().initProto();
        state_pr.setState(ComponentStateProto::State::ERROR);
        ToProto(this->compError, state_pr.initError());
        tx_state().publish();

        // Stop the recv handler
        this->recv_handler->stop();
        this->can_if->disconnect();
      },
      [this] { this->sendBatteryStateProto(); }, [] {});

  machine_.addState(
      kStateOperational,
      [this] {
        // publish state operational
        auto state_pr = tx_state().initProto();
        state_pr.setState(ComponentStateProto::State::OPERATIONAL);
        tx_state().publish();

        // Create the operational thread
        this->running = std::promise<void>();
        this->op_thread = make_unique<std::thread>(op_main, this, this->running.get_future());
        if ((get_core_idx() > 0) &&
            (static_cast<uint>(get_core_idx()) < std::thread::hardware_concurrency())) {
          cpu_set_t core_set;
          CPU_ZERO(&core_set);
          CPU_SET(get_core_idx(), &core_set);
          pthread_setaffinity_np(this->op_thread->native_handle(), sizeof(cpu_set_t), &core_set);
        }
      },
      [this] {}, [this] { 
        this->running.set_value(); 
        this->op_thread->join();
        });

  machine_.addTransition(
      kStateInit, kStateCheckConnection, [this] { return this->isInitialized; }, [this] {});

  machine_.addTransition(
      kStateInit, kStateError, [this] { return this->isError; }, [this] {});

  machine_.addTransition(
      kStateCheckConnection, kStateReset, [this] { return this->my_bms->isConnected(); },
      [this] {});

  machine_.addTransition(
      kStateCheckConnection, kStateError,
      [this] { return ((this->getTickTime() - this->transitionTickTime) > 1); },
      [this] { this->compError = bms_error_map.at("MIDLBMSER0002M"); });

  machine_.addTransition(
      kStateReset, kStateWaitForReset,
      [this] { return ((this->getTickTime() - this->transitionTickTime) > RESET_WAIT_TIME_S); },
      [this] {});

  machine_.addTransition(
      kStateWaitForReset, kStateReset,
      [this] {
        return ((this->resetCounter != 0) && (!this->my_bms->isMSWOn()) &&
                ((this->getTickTime() - this->transitionTickTime) > AFTER_RESET_WAIT_TIME_S));
      },
      [this] { this->resetCounter--; });

  machine_.addTransition(
      kStateWaitForReset, kStateError,
      [this] {
        return ((this->resetCounter == 0) && (!this->my_bms->isMSWOn()) &&
                ((this->getTickTime() - this->transitionTickTime) > AFTER_RESET_WAIT_TIME_S));
      },
      [this] { this->compError = bms_error_map.at("MKNXVEHCL1500M"); });

  machine_.addTransition(
      kStateWaitForReset, kStateOperational, [this] { return this->my_bms->isMSWOn(); }, [] {});

  machine_.addTransition(
      kStateOperational, kStateError,
      [this] { return ((!this->my_bms->isMSWOn()) || (!this->my_bms->isConnected())); },
      [this] {
        this->compError = bms_error_map.at("MIDLBMSER0004M");
        this->compError.description += this->errorMessages;
        this->errorMessages = "";
      });
}

void Bms::sendBatteryStateProto() {
  auto proto = tx_battery_state().initProto();
  proto.setState(this->my_bms->getState());
  proto.setUBatt(this->my_bms->getUBatt());
  proto.setIBatt(this->my_bms->getIBatt());
  proto.setStateOfCharge(this->my_bms->getSOC());
  proto.setCellAvgmV(this->my_bms->getCellAvgMV());
  proto.setCellMaxmV(this->my_bms->getCellMaxMV());
  proto.setCellMinmV(this->my_bms->getCellMinMV());
  proto.setCellMaxtemp(this->my_bms->getCellTempMax());
  proto.setCellMintemp(this->my_bms->getCellTempMin());
  proto.setPosChargecur(this->my_bms->getPosChargeCur());
  proto.setPosChargepow(this->my_bms->getPosChargePow());
  proto.setPosDischargecur(this->my_bms->getPosDischargeCur());
  proto.setPosDischargepow(this->my_bms->getPosDischargePow());
  proto.setCellChargingRes(this->my_bms->getCellRChargeMO());
  proto.setCellDischargingRes(this->my_bms->getCellRDischargeMO());
  proto.setCellChargingApproxRes(this->my_bms->getCellRChargeMO());
  proto.setCellDischargingApproxRes(this->my_bms->getCellRDischargeMOA());
  proto.setBatteryVoltage(this->my_bms->getBatteryVoltageV());
  proto.setIsolationResistance(this->my_bms->getIsolationRKO());
  proto.setUnderCurrentTimer(this->my_bms->getTCur());
  proto.setCapacityValue(this->my_bms->getCapacityValueAS());
  if (this->my_bms->getIBatt() <= 0) {
    proto.setIsCharging(1);
  } else {
    proto.setIsCharging(0);
  }

  if ((this->my_bms->getUBatt() <= this->get_minBatteryVoltage()) ||
      (this->my_bms->getSOC() <= this->get_minSOC()) ||
      (this->my_bms->getCellMinMV() <= this->get_minCellVoltage())) {
    proto.setDischargeWarning(1);
  } else {
    proto.setDischargeWarning(0);
  }

  if (this->my_bms->errors.size() > 0) {
    EvaBMSError e = this->my_bms->errors[0];
    if (!(this->my_bms->errors[0].event.compare("ePMB_ERR_BROKEN_WIRE") == 0) &&
        !(this->my_bms->errors[0].event.compare("eSC_ERR_PMB_WARNING") == 0)) {
      LOG_ERROR("%s, %s, %s", e.event.c_str(), e.message.c_str(), e.category.c_str());
      proto.setHasError(1);
      proto.setErrorMsg(e.message);
      this->errorMessages += " | " + e.message;
    }
    this->my_bms->errors.erase(this->my_bms->errors.begin());
  } else {
    proto.setHasError(0);
  }

  tx_battery_state().publish();
  show("battery.Ibatt", this->my_bms->getIBatt());
  show("battery.Ubatt", this->my_bms->getUBatt());
  show("battery.SOC", this->my_bms->getSOC());
  show("battery.msw", static_cast<int>(this->my_bms->isMSWOn()));
  // LOG_INFO("SOC: %.3f, I_batt: %.3f, U_batt:%.3f, Cell_avg_mv:%d, state: %8.8x ",
  //          this->my_bms->getSOC(), this->my_bms->getIBatt(), this->my_bms->getUBatt(),
  //          this->my_bms->getCellAvgMV(), this->my_bms->getState());
}

}  // namespace str