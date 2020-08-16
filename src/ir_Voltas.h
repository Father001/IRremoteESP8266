// Copyright 2020 David Conran (crankyoldgit)
// Copyright 2020 manj9501
/// @file
/// @brief Support for Voltas A/C protocol
/// @see https://github.com/crankyoldgit/IRremoteESP8266/issues/1238

// Supports:
//   Brand: Voltas,  Model: 122LZF 4011252 Window A/C
//
// Ref: https://docs.google.com/spreadsheets/d/1zzDEUQ52y7MZ7_xCU3pdjdqbRXOwZLsbTGvKWcicqCI/
// Ref: https://www.corona.co.jp/box/download.php?id=145060636229

#ifndef IR_VOLTAS_H_
#define IR_VOLTAS_H_

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include "IRremoteESP8266.h"
#include "IRsend.h"
#ifdef UNIT_TEST
#include "IRsend_test.h"
#endif


union VoltasProtocol {
  uint8_t raw[kVoltasStateLength];  ///< The state in native IR code form
  struct {
    // Byte 0
    uint8_t SwingH         :1;
    uint8_t Unknown0       :7;
    // Byte 1
    uint8_t Mode           :4;
    uint8_t                :1;
    uint8_t FanSpeed       :3;
    // Byte 2
    uint8_t SwingV         :3;
    uint8_t Wifi           :1;
    uint8_t                :1;
    uint8_t Turbo          :1;
    uint8_t Sleep          :1;
    uint8_t Power          :1;
    // Byte 3
    uint8_t Temp           :4;
    uint8_t Unknown3       :2;  // Typically 0b01
    uint8_t Econo          :1;
    uint8_t TempSet        :1;
    // Byte 4
    uint8_t OffTimer24h4   :1;
    uint8_t                :7;  // Typically 0b0011101
    // Byte 5
    uint8_t OffTimer24h5   :1;
    uint8_t                :6;  // Typically 0b011101
    uint8_t TimerAdd12Hr   :1;
    // Byte 6
    uint8_t                :8;  // Typically 0b00111011(0x3B)
    // Byte 7
    uint8_t                :4;  // Typically 0b0001
    uint8_t TimerHrs       :4;  // Nr of Hours.
    // Byte 8
    uint8_t                :5;  // Typically 0b00000
    uint8_t Light          :1;
    uint8_t OffTimerEnable :1;
    uint8_t                :1;  // Typically 0b0
    // Byte 9
    uint8_t Checksum       :8;
  };
};

// Constants

// Classes
/// Class for handling detailed Voltas A/C messages.
class IRVoltas {
 public:
  explicit IRVoltas(const uint16_t pin, const bool inverted = false,
                    const bool use_modulation = true);
  void stateReset();
#if SEND_VOLTAS
  void send(const uint16_t repeat = kNoRepeat);
  /// Run the calibration to calculate uSec timing offsets for this platform.
  /// @return The uSec timing offset needed per modulation of the IR Led.
  /// @note This will produce a 65ms IR signal pulse at 38kHz.
  ///   Only ever needs to be run once per object instantiation, if at all.
  int8_t calibrate(void) { return _irsend.calibrate(); }
#endif  // SEND_VOLTAS
  void begin();
  static bool validChecksum(const uint8_t state[],
                            const uint16_t length = kVoltasStateLength);
  void setPower(const bool on);
  bool getPower(void) const;
  void on(void);
  void off(void);
  void setTemp(const uint8_t temp);
  uint8_t getTemp(void);
  void setFan(const uint8_t speed);
  uint8_t getFan(void);
  void setMode(const uint8_t mode);
  uint8_t getMode(void);
  void setEcono(const bool on);
  bool getEcono(void);
  void setOffTimer(const uint16_t nr_of_mins);
  uint16_t getOffTimer(void);
  uint8_t* getRaw(void);
  void setRaw(const uint8_t new_code[]);
  uint8_t convertMode(const stdAc::opmode_t mode);
  uint8_t convertFan(const stdAc::fanspeed_t speed);
  static stdAc::opmode_t toCommonMode(const uint8_t mode);
  static stdAc::fanspeed_t toCommonFanSpeed(const uint8_t speed);
  stdAc::state_t toCommon(void);
  String toString(void);
#ifndef UNIT_TEST

 private:
  IRsend _irsend;  ///< Instance of the IR send class
#else
  /// @cond IGNORE
  IRsendTest _irsend;  ///< Instance of the testing IR send class
  /// @endcond
#endif
  VoltasProtocol _;  ///< The state of the IR remote.
  void checksum(void);
  static uint8_t calcChecksum(const uint8_t state[],
                              const uint16_t length = kVoltasStateLength);
};
#endif  // IR_VOLTAS_H_
