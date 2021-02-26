"""
Copyright (c) 2019, BMW Group. All rights reserved.
BMW Group and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from BMW Group is strictly prohibited.
"""

load("@com_nvidia_isaac_sdk//bzl:module.bzl", "isaac_app")
load("@com_nvidia_isaac_sdk//bzl:module.bzl", "isaac_component")
load("@com_nvidia_isaac_sdk//bzl:module.bzl", "isaac_cc_module")
load("@com_nvidia_isaac_engine//engine/build:cc_capnp_library.bzl", "cc_capnp_library")

cc_capnp_library(
    name = "batt_state_proto",
    protos = [
        "battery_state.capnp",
    ],
)

isaac_component(
    name = "can_recv_handler",
    visibility = ["//visibility:public"],
    copts = ["-Ilibraries/CAN"],
    deps = [
        "//libraries/CAN/can-lib",
    ],
)

isaac_component(
    name = "bms_component",
    copts = ["-Ilibraries/CAN"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_nvidia_isaac_sdk//packages/engine_gems/state:io",
        "@com_nvidia_isaac_engine//engine/gems/state_machine",
        "//libraries/CAN",
        "//messages",
        ":batt_state_proto",
        ":can_recv_handler",
        ],
)

isaac_cc_module(
    name = "bms_module",
    deps = [
        ":can_recv_handler",
        ":bms_component",
        ],
)

isaac_app(
    name = "bms",
    app_json_file = "bms.app.json",
    modules = [
        "//components/bms:bms_module",
    ],
)