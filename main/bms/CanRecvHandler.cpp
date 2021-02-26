/*
Copyright (c) 2019, BMW Group. All rights reserved.
BMW Group and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from BMW Group is strictly prohibited.
*/

#include "CanRecvHandler.hpp"

using namespace std;

namespace str {

void CanRecvHandler::start() {
  tickPeriodically();
}

void CanRecvHandler::tick() {
  this->can_if->readDataBlocking();
}

void CanRecvHandler::stop() {}

}  // namespace str