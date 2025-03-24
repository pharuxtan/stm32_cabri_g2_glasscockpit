const dataref = @import("./dataref.zig");
const usb = @import("./usb.zig");

pub const XPlaneIP = "127.0.0.1";
pub const XPlaneSendPort = 49000;
pub const XPlaneRcvPort = 49001;

pub const usbVendorID = 0x4854;
pub const usbProductID = 0x4743;
pub const usbCDCEndOut = 0x02;
pub const usbCDCInfNum = 0x01;
pub const usbCDCSerial = usb.LineEncoding{
  .BaudRate = 115200,
  .StopBits = .@"1",
  .Parity = .None,
  .DataBits = .@"8"
};

pub const DataRefFrequency = 100; // Hz

pub const USBDataEnum = enum(u8) {
  CARBURATOR_TEMP       = 0x00,
  BATTERY_VOLTAGE       = 0x01,
  OUTSIDE_AIR_TEMP      = 0x02,
  EXHAUST_GAS_TEMP      = 0x03,
  FUEL_QUANTITY         = 0x04,
  FUEL_FLOW             = 0x05,
  CYLINDER_HEAD_TEMP    = 0x06,
  OIL_TEMP              = 0x07,
  ENGN_OIL_TEMP         = 0x08,
  OIL_PRESSURE          = 0x09,
  ENGN_OIL_PRESSURE     = 0x0A,
  FUEL_PRESSURE         = 0x0B,
  ROTOR_PWR             = 0x0C,
  ROTOR_RPM             = 0x0D,
  ENGINE_TORQUE         = 0x0E,
  ENGINE_RPM            = 0x0F,
  LOCAL_TIME_HOURS      = 0x10,
  LOCAL_TIME_MINUTES    = 0x11,
  CARBURATOR_TEMP_LEVEL = 0xA0,
  FLIGHT_TIME           = 0xA1,
  ENGINE_FIRE           = 0xA2,
  CRASHED               = 0xA3,
  PAUSED                = 0xA4,
  POWER                 = 0xFF,
};

pub const datarefs: []const dataref.DataRef = &[_]dataref.DataRef{
  .{ .name = "sim/cockpit2/electrical/bus_volts[0]", .key = .POWER, .T = .uint8 },
  .{ .name = "vskylabs/g2/carbheat_level", .key = .CARBURATOR_TEMP_LEVEL, .T = .uint8 },
  .{ .name = "sim/cockpit2/annunciators/engine_fire", .key = .ENGINE_FIRE, .T = .uint8 },
  .{ .name = "sim/flightmodel2/misc/has_crashed", .key = .CRASHED, .T = .uint8 },
  .{ .name = "sim/time/paused", .key = .PAUSED, .T = .uint8 },
  .{ .name = "vskylabs/g2/carbair_temp", .key = .CARBURATOR_TEMP, .T = .float32 },
  .{ .name = "sim/cockpit2/electrical/battery_voltage_indicated_volts[0]", .key = .BATTERY_VOLTAGE, .T = .float32 },
  .{ .name = "sim/cockpit2/temperature/outside_air_temp_degc", .key = .OUTSIDE_AIR_TEMP, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/EGT_deg_C[0]", .key = .EXHAUST_GAS_TEMP, .T = .float32 },
  .{ .name = "sim/cockpit2/fuel/fuel_quantity[0]", .key = .FUEL_QUANTITY, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/fuel_flow_kg_sec[0]", .key = .FUEL_FLOW, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/CHT_deg_C[0]", .key = .CYLINDER_HEAD_TEMP, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/oil_temperature_deg_C[0]", .key = .OIL_TEMP, .T = .float32 },
  .{ .name = "sim/flightmodel/engine/ENGN_oil_temp_c[0]", .key = .ENGN_OIL_TEMP, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/oil_pressure_psi[0]", .key = .OIL_PRESSURE, .T = .float32 },
  .{ .name = "sim/flightmodel/engine/ENGN_oil_press_psi[0]", .key = .ENGN_OIL_PRESSURE, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/fuel_pressure_psi[0]", .key = .FUEL_PRESSURE, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/actuators/prop_ratio[0]", .key = .ROTOR_PWR, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/prop_speed_rpm[0]", .key = .ROTOR_RPM, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/torque_n_mtr[0]", .key = .ENGINE_TORQUE, .T = .float32 },
  .{ .name = "sim/cockpit2/engine/indicators/engine_speed_rpm[0]", .key = .ENGINE_RPM, .T = .float32 },
  .{ .name = "sim/cockpit2/clock_timer/local_time_hours", .key = .LOCAL_TIME_HOURS, .T = .float32 },
  .{ .name = "sim/cockpit2/clock_timer/local_time_minutes", .key = .LOCAL_TIME_MINUTES, .T = .float32 },
  .{ .name = "sim/time/total_flight_time_sec", .key = .FLIGHT_TIME, .T = .uint64 },
};
