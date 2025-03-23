const std = @import("std");
const network = @import("network");

const allocator = std.heap.page_allocator;

var sock: network.Socket = undefined;

pub fn init() !void {
  try network.init();

  sock = try network.Socket.create(.ipv4, .udp);
  
  const addr = network.EndPoint{
    .address = network.Address{ .ipv4 = network.Address.IPv4.any },
    .port = 0
  };

  try sock.bind(addr);
}

pub fn deinit() void {
  sock.close();
  network.deinit();
}

pub fn connect(address: []const u8, port: u16) !void {
  const endpoint_list = try network.getEndpointList(allocator, address, port);
  defer endpoint_list.deinit();

  for (endpoint_list.endpoints) |endpt| {
    sock.connect(endpt) catch continue;
    break;
  }
}

pub fn sendTo(address: []const u8, port: u16, data: []const u8) !void {
  const endpoint_list = try network.getEndpointList(allocator, address, port);
  defer endpoint_list.deinit();

  for (endpoint_list.endpoints) |endpt| {
    _ = sock.sendTo(endpt, data) catch continue;
    break;
  }
}

pub fn receive(data: []u8) !usize {
  return try sock.receive(data);
}
