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

  var askThread = try std.Thread.spawn(.{}, datarefAskThread, .{});
  askThread.detach();

  while(true){
    const datarefs = try dataref.receive();
    try dataref.send(datarefs);
  }
}

fn datarefAskThread() void {
  while(true){
    dataref.ask() catch {};
    std.Thread.sleep(1e9); // Ask every 1 second
  }
}
