const std = @import("std");
const libusb = @import("libusb");
const config = @import("./config.zig");

const ACM_CTRL_DTR = 0x01;
const ACM_CTRL_RTS = 0x02;

var dev: ?*libusb.libusb_device_handle = null;
var connected: bool = false;

pub const LineEncoding = extern struct {
  BaudRate: u32 align(1),
  StopBits: enum(u8) { @"1", @"1.5", @"2" } align(1),
  Parity: enum(u8) { None, Odd, Even, Mark, Space } align(1),
  DataBits: enum(u8) { @"5" = 5, @"6" = 6, @"7" = 7, @"8" = 8, @"16" = 16 } align(1)
};

pub fn is_connected() bool {
  return connected;
}

pub fn init() !void {
  const rc = libusb.libusb_init(null);

  if (rc < 0) {
    std.debug.print("Error initializing libusb, {s}\n", .{libusb.libusb_error_name(rc)});
    return error.ErrorInitLibUSB;
  }
}

pub fn connect() !void {
  dev = libusb.libusb_open_device_with_vid_pid(null, config.usbVendorID, config.usbProductID);

  if (dev == null)
    return error.ErrorFindingUSBDevice;

  if (libusb.libusb_kernel_driver_active(dev, config.usbCDCInfNum) != 0)
    _ = libusb.libusb_detach_kernel_driver(dev, config.usbCDCInfNum);

  var rc = libusb.libusb_claim_interface(dev, config.usbCDCInfNum);

  if (rc < 0) {
    std.debug.print("Error claiming interface: {s}\n", .{libusb.libusb_error_name(rc)});
    return error.ErrorClaimingInterface;
  }

  rc = libusb.libusb_control_transfer(dev, 0x21, 0x22, ACM_CTRL_DTR | ACM_CTRL_RTS, 0, null, 0, 0);

  if (rc < 0) {
    std.debug.print("Error during control transfer: {s}\n", .{libusb.libusb_error_name(rc)});
    return error.ErrorControlTransferState;
  }

  var encoding = std.mem.toBytes(config.usbCDCSerial);

  rc = libusb.libusb_control_transfer(dev, 0x21, 0x20, 0, 0, &encoding, encoding.len, 0);

  if (rc < 0) {
    std.debug.print("Error during control transfer: {s}\n", .{libusb.libusb_error_name(rc)});
    return error.ErrorControlTransferParams;
  }

  connected = true;
}

pub fn deinit() void {
  connected = false;
  if (dev) |_| {
    _ = libusb.libusb_release_interface(dev, config.usbCDCInfNum);
    libusb.libusb_close(dev);
  }
  libusb.libusb_exit(null);
}

pub fn try_connect() void {
  if (dev == null) {
    connect() catch return;
  }
}

pub fn send(data: []u8) !void {
  var rc: c_int = 0;

  try_connect();

  if (dev != null)
    rc = libusb.libusb_bulk_transfer(dev, config.usbCDCEndOut, data.ptr, @intCast(data.len), null, 0);

  if (rc < 0) {
    dev = null;
    connected = false;
    return;
  }
}
