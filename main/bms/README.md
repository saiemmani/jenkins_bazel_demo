
## str.Bms
#### Files 
[BmsComponent.hpp](./BmsComponent.hpp) 
#### Description 
 


#### Incoming messages 
- force_shutdown [`BoolProto`]: Force shutdown the BMS


#### Outgoing messages 
- battery_state [`BatteryStateProto`]: State of the STR Battery containing all battery information

- state [`ComponentStateProto`]: 


#### Parameters 
- if_name [`std::string`][default="can0"]: Name of the can interface the BMS is connected to

- minBatteryVoltage [`int`][default=40]: Minimum battery voltage before sending a discharge warning

- minSOC [`int`][default=40]: Minimum State of charge before sending a discharge warning

- minCellVoltage [`double`][default=3.5]: Minimum Minimum cell voltage before sending a discharge warning

- request_shutdown [`bool`][default=false]: Force request shutdown of the main switch of the BMS

- core_idx [`int`][default=-1]: Number of the core to run the main thread on, -1 for not specified


--- 

## str.CanRecvHandler
#### Files 
[CanRecvHandler.hpp](./CanRecvHandler.hpp) 
#### Description 
 


#### Incoming messages 

#### Outgoing messages 

#### Parameters 
- bms_node [`std::string`]: 


--- 
