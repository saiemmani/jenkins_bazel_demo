@0xc092de16de90d9cd; #Auto generated

struct BatteryStateProto{
    #State of the battery
    state @0 : UInt32;
    #Battery voltage in Volts
    uBatt @1 : Float64; 
    #Battery current in Amps
    iBatt @2 : Float64;
    #Battery state of charge
    stateOfCharge @3 : Float64;
    #Cell average voltage in mV
    cellAvgmV @4 : UInt16;
    #cell minimum voltage in mV
    cellMinmV @5 : UInt16;
    #cell maximum voltage in mV
    cellMaxmV @6 : UInt16;
    #cell maximum temperature
    cellMaxtemp @7 : Int8;
    #cell minimum temperature
    cellMintemp @8 : Int8;
    #Possible charge current
    posChargecur @9 : Float64;
    #Possible charge power
    posChargepow @10 : UInt16;
    #Possible discharge current
    posDischargecur @11 : Float64;
    #Possible discharge power
    posDischargepow @12 : UInt16;
    #Cell Charging resistance
    cellChargingRes @13 : Float64;
    #Cell Discharging resistance
    cellDischargingRes @14 : Float64;
    #Cell Charging approximate resistance
    cellChargingApproxRes @15 : Float64;
    #Cell Discharging approximate resistance
    cellDischargingApproxRes @16 : Float64;
    #Battery voltage
    batteryVoltage @17 : Float64;
    #Isolation resistance
    isolationResistance @18 : UInt16;
    #Under current timer
    underCurrentTimer @19 : UInt32;
    #Capacity Value
    capacityValue @20 : UInt32;
    #Is charging
    isCharging @21 : UInt8;
    #Discharge Warning
    dischargeWarning @22 : UInt8;
    #Has error
    hasError @23 : UInt8;
    #Error Message
    errorMsg @24 : Text; 
    #On/Off State of Battery
    isOn @25 : UInt8;
}
