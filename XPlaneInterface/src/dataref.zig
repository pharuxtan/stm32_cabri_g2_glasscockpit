const std = @import("std");
const udp = @import("./udp.zig");
const usb = @import("./usb.zig");
const config = @import("./config.zig");

const datarefs = config.datarefs;

var requestsMessages: [datarefs.len]DataRefRequest = undefined;

pub fn init() !void {
  try udp.connect(config.XPlaneIP, config.XPlaneRcvPort);

  for(datarefs, 0..) |dataref, i| {
    var request: DataRefRequest = .{ .index = @intCast(i) };

    std.mem.copyForwards(u8, request.name[0..dataref.name.len], dataref.name);
    
    requestsMessages[i] = request;
  }
}

pub fn deinit() void {
  unsubscribe() catch {};
}

pub fn subscribe() !void {
  for(requestsMessages) |message| {
    const buf = std.mem.toBytes(message);
    try udp.sendTo(config.XPlaneIP, config.XPlaneSendPort, &buf);
  }
}

pub fn unsubscribe() !void {
  for(requestsMessages) |*message| {
    message.frequency = 0;
    const buf = std.mem.toBytes(message.*);
    try udp.sendTo(config.XPlaneIP, config.XPlaneSendPort, &buf);
  }
}

var rcvValues: [datarefs.len]DataRefValue = undefined;

pub fn receive() ![]DataRefValue {
  var buf: [5 + 8 * datarefs.len]u8 = undefined;

  const bytes = try udp.receive(&buf);

  var fbs = std.io.fixedBufferStream(buf[0..bytes]);
  const reader = fbs.reader();

  const header = try reader.readStruct(DataRefHeader);
  if(std.mem.eql(u8, &header.magic, "RREF")){
    const N = (bytes - fbs.pos) / 8;
    for(0..N) |i| {
      rcvValues[i] = try reader.readStruct(DataRefValue);
    }
    return rcvValues[0..N];
  }

  return rcvValues[0..0];
}

pub fn send(values: []DataRefValue) !void {
  var buf: [9 * datarefs.len]u8 = undefined;
  var fbs = std.io.fixedBufferStream(buf[0..]);

  var total: usize = 0;

  for(values) |dref| {
    const ref = datarefs[dref.index];

    var data: [9]u8 = undefined;

    const size: usize = switch(ref.T){
      .uint8   => try createUSBData(u8,  &data, ref.key, @intFromFloat(dref.value)),
      .uint16  => try createUSBData(u16, &data, ref.key, @intFromFloat(dref.value)),
      .uint32  => try createUSBData(u32, &data, ref.key, @intFromFloat(dref.value)),
      .uint64  => try createUSBData(u64, &data, ref.key, @intFromFloat(dref.value)),

      .int8    => try createUSBData(i8,  &data, ref.key, @intFromFloat(dref.value)),
      .int16   => try createUSBData(i16, &data, ref.key, @intFromFloat(dref.value)),
      .int32   => try createUSBData(i32, &data, ref.key, @intFromFloat(dref.value)),
      .int64   => try createUSBData(i64, &data, ref.key, @intFromFloat(dref.value)),
      
      .float32 => try createUSBData(f32, &data, ref.key, dref.value),
      .float64 => try createUSBData(f64, &data, ref.key, @floatCast(dref.value)),
    };

    total += try fbs.write(data[0..size]);
  }
  
  try usb.send(buf[0..total]);
}

fn createUSBData(comptime T: type, buf: []u8, key: config.USBDataEnum, val: T) !usize {
  const data: extern struct {
    key: u8 align(1),
    val: T align(1),
  } = .{
    .key = @intFromEnum(key),
    .val = val
  };

  var fbs = std.io.fixedBufferStream(buf);
  try fbs.writer().writeStruct(data);

  return 1 + @sizeOf(T);
}

pub const DataRefRequest = extern struct {
    magic: [5]u8 align(1) = .{ 'R', 'R', 'E', 'F', '\x00' },
    frequency: u32 align(1) = config.DataRefFrequency,
    index: u32 align(1),
    name: [400]u8 align(1) = .{ '\x00' } ** 400
};

pub const DataRefHeader = extern struct {
  magic: [4]u8 align(1),
  internal: u8 align(1),
};

pub const DataRefValue = extern struct {
  index: u32 align(1),
  value: f32 align(1)
};

pub const DataRefValTypeEnum = enum {
  uint8,
  uint16,
  uint32,
  uint64,

  int8,
  int16,
  int32,
  int64,

  float32,
  float64,
};

pub const DataRef = struct {
  name: []const u8,
  key: config.USBDataEnum,
  T: DataRefValTypeEnum
};