/*
Copyright (c) 2019, BMW Group. All rights reserved.
BMW Group and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from BMW Group is strictly prohibited.
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "engine/alice/alice_codelet.hpp"
#include "engine/alice/components/Failsafe.hpp"
#include "messages/messages.hpp"

#include "libraries/CAN/can-lib/CanInterface.hpp"

using namespace bmw_can;

namespace str {
class CanRecvHandler : public isaac::alice::Codelet {
 public:
  void start() override;
  void tick() override;
  void stop() override;
  ISAAC_PARAM(std::string, bms_node);

  CanInterface* can_if;
};
}  // namespace str

ISAAC_ALICE_REGISTER_CODELET(str::CanRecvHandler);