#ifndef USB_H
#define USB_H

#include "app/app.h"
#include "tusb.h"

#define USB_DATA_BUF_SIZE 512

typedef enum __attribute__((packed)) {
  // float
  USB_FLOAT_START       = 0x00,
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

  // u8
  CARBURATOR_TEMP_LEVEL = 0xA0,
  FLIGHT_TIME           = 0xA1,
  ENGINE_FIRE           = 0xA2,
  CRASHED               = 0xA3,
  PAUSED                = 0xA4,
  POWER                 = 0xFF,
} usb_data_types_t;

typedef struct __attribute__((packed)) {
  usb_data_types_t data_type;
   union {
     uint8_t  u8;
     uint16_t u16;
     uint32_t u32;
     uint64_t u64;

     int8_t   i8;
     int16_t  i16;
     int32_t  i32;
     int64_t  i64;

     float    f32;
     double   f64;
   } data;
} usb_data_t;

void usb_init();

#endif
