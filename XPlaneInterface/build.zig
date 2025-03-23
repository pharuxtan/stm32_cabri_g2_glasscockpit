const std = @import("std");

pub fn build(b: *std.Build) void {
  const dev = b.option(bool, "dev", "Run the application in development mode") orelse false;

  const optimize: std.builtin.OptimizeMode = if(dev) .Debug else .ReleaseSafe;

  const target = b.standardTargetOptions(.{});

  const exe_mod = b.createModule(.{
    .root_source_file = b.path("src/main.zig"),
    .target = target,
    .optimize = optimize,
  });
  
  const exe = b.addExecutable(.{
    .name = "xplane_interface_glasscockpit",
    .root_module = exe_mod
  });

  exe.subsystem = .Windows;

  const libusb = b.addStaticLibrary(.{
    .name = "libusb",
    .target = target,
    .optimize = optimize
  });

  const libusb_dep = b.dependency("libusb", .{});

  const cflags: []const []const u8 = &.{
    "-O2",
    "-Winline",
    "-Wmissing-include-dirs",
    "-Wnested-externs",
    "-Wpointer-arith",
    "-Wredundant-decls",
    "-Wswitch-enum",
    "-fno-omit-frame-pointer",
    "-D_MSC_VER=1800",
    "-D_MSC_EXTENSIONS",
    "-D__CRT__NO_INLINE",
  };

  libusb.addCSourceFiles(.{
    .root = libusb_dep.path(""),
    .files = &.{
      "libusb/core.c",
      "libusb/descriptor.c",
      "libusb/hotplug.c",
      "libusb/io.c",
      "libusb/strerror.c",
      "libusb/sync.c",
      
      "libusb/os/events_windows.c",
      "libusb/os/threads_windows.c",
      "libusb/os/windows_common.c",
      "libusb/os/windows_usbdk.c",
      "libusb/os/windows_winusb.c",
    },
    .flags = cflags
  });

  libusb.addIncludePath(libusb_dep.path("libusb"));
  libusb.addIncludePath(libusb_dep.path("libusb/os"));
  libusb.addIncludePath(libusb_dep.path("msvc"));

  libusb.linkLibC();

  exe.linkLibrary(libusb);

  exe.step.dependOn(&libusb.step);

  const translate = b.addTranslateC(.{
    .target = target,
    .optimize = optimize,
    .root_source_file = libusb_dep.path("libusb/libusb.h")
  });

  const libusb_translate = translate.addModule("libusb");
  exe.root_module.addImport("libusb", libusb_translate);

  libusb.step.dependOn(&translate.step);

  const network_mod = b.dependency("network", .{}).module("network");
  exe.root_module.addImport("network", network_mod);

  const install = b.addInstallArtifact(exe, .{
    .dest_dir = .{
      .override = .{
        .custom = if(optimize == .Debug) "../build/Debug" else "../build/Release",
      },
    },
    .pdb_dir = if(optimize == .Debug) .default else .disabled,
  });

  b.getInstallStep().dependOn(&install.step);
}
