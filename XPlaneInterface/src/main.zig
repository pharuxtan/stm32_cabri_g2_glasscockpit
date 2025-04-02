const std = @import("std");
const usb = @import("./usb.zig");
const udp = @import("./udp.zig");
const dataref = @import("./dataref.zig");

pub fn main() !void {
  try usb.init();
  defer usb.deinit();

  try udp.init();
  defer udp.deinit();

  try dataref.init();
  defer dataref.deinit();

  var subscribeThread = try std.Thread.spawn(.{}, datarefSubscribeThread, .{});
  subscribeThread.detach();

  while(true){
    const datarefs = try dataref.receive();
    try dataref.send(datarefs);
  }
}

fn datarefSubscribeThread() void {
  while(true){
    dataref.subscribe() catch {};
    std.Thread.sleep(1e9); // Subscribe every 1 second
  }
}
