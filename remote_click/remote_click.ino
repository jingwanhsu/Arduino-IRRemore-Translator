/*
 * ReceiveAndSend.cpp
 *
 * Record and play back last received IR signal at button press.
 * The logic is:
 * If the button is pressed, send the IR code.
 * If an IR code is received, record it.
 *
 * An example for simultaneous receiving and sending is in the UnitTest example.
 *
 * An IR detector/demodulator must be connected to the input IR_RECEIVE_PIN.
 *
 * A button must be connected between the input SEND_BUTTON_PIN and ground.
 * A visible LED can be connected to STATUS_PIN to provide status.
 *
 *
 * Initially coded 2009 Ken Shirriff http://www.righto.com
 *
 *  This file is part of Arduino-IRremote
 *https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2009-2021 Ken Shirriff, Armin Joachimsmeyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 *
 ************************************************************************************
 */
#include <Arduino.h>

/*
 * Define macros for input and output pin etc.
 */
#include "IRCodeCDPlayerRemote.h"
#include "IRCodeTVRemote.h"
#include "IRRemoteTranslator.h"
#include "PinDefinitionsAndMore.h"

//#define EXCLUDE_EXOTIC_PROTOCOLS // saves around 900 bytes program space

// On the Zero and others we switch explicitly to SerialUSB
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

void setup() {
  Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || \
    defined(SERIAL_PORT_USBVIRTUAL) || defined(ARDUINO_attiny3217)
  delay(4000);  // To be able to connect Serial monitor after reset or power up
                // and before first print out. Do not wait for an attached
                // Serial Monitor!
#endif
  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__
                   "\r\nUsing library version " VERSION_IRREMOTE));

  IrReceiver.begin(IR_RECEIVE_PIN,
                   ENABLE_LED_FEEDBACK);  // Start the receiver, enable feedback
                                          // LED, take LED feedback pin from the
                                          // internal boards definition

  IrSender.begin(IR_SEND_PIN,
                 ENABLE_LED_FEEDBACK);  // Specify send pin and enable feedback
                                        // LED at default feedback LED pin

  pinMode(STATUS_PIN, OUTPUT);

  Serial.print(F("Ready to receive IR signals at pin "));
#if defined(ARDUINO_ARCH_STM32) || defined(ESP8266)
  Serial.println(IR_RECEIVE_PIN_STRING);
#else
  Serial.println(IR_RECEIVE_PIN);
#endif

  Serial.print(F("Ready to send IR signals at pin "));
#if defined(ARDUINO_ARCH_STM32) || defined(ESP8266)
  Serial.println(IR_SEND_PIN_STRING);
#else
  Serial.println(IR_SEND_PIN);
#endif
}

void loop() {
  IrTranslator.begin();
  IrTranslator.translate({
      .name = "Hulu Subtitles",
      .original = {.address = CD_REMOTE_ADDRESS, .command = CD_REMOTE_TOP_LEFT},
      .translation =
          {
              .length = 7,
              .codeInfos =
                  {
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_UP},
                       .delay = 100},
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_RIGHT},
                       .delay = 100},
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_RIGHT},
                       .delay = 100},
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_OK},
                       .delay = 100},
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_OK},
                       .delay = 100},
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_LEFT},
                       .delay = 300},
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_UP},
                       .delay = 0},
                  },
          },
  });
  IrTranslator.translate({
      .name = "TV Up",
      .original = {.address = CD_REMOTE_ADDRESS, .command = CD_REMOTE_UP},
      .translation =
          {
              .length = 1,
              .codeInfos =
                  {
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_UP},
                       .delay = 0},
                  },
          },
  });
  IrTranslator.translate({
      .name = "TV Down",
      .original = {.address = CD_REMOTE_ADDRESS, .command = CD_REMOTE_DOWN},
      .translation =
          {
              .length = 1,
              .codeInfos =
                  {
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_DOWN},
                       .delay = 0},
                  },
          },
  });
  IrTranslator.translate({
      .name = "TV Left",
      .original = {.address = CD_REMOTE_ADDRESS, .command = CD_REMOTE_LEFT},
      .translation =
          {
              .length = 1,
              .codeInfos =
                  {
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_LEFT},
                       .delay = 0},
                  },
          },
  });
  IrTranslator.translate({
      .name = "TV Right",
      .original = {.address = CD_REMOTE_ADDRESS, .command = CD_REMOTE_RIGHT},
      .translation =
          {
              .length = 1,
              .codeInfos =
                  {
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_RIGHT},
                       .delay = 0},
                  },
          },
  });
  IrTranslator.translate({
      .name = "TV OK",
      .original = {.address = CD_REMOTE_ADDRESS, .command = CD_REMOTE_OK},
      .translation =
          {
              .length = 1,
              .codeInfos =
                  {
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_OK},
                       .delay = 0},
                  },
          },
  });
  IrTranslator.translate({
      .name = "TV Back",
      .original = {.address = CD_REMOTE_ADDRESS, .command = CD_REMOTE_STOP},
      .translation =
          {
              .length = 1,
              .codeInfos =
                  {
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_BACK},
                       .delay = 0},
                  },
          },
  });
  IrTranslator.translate({
      .name = "TV Mute",
      .original = {.address = CD_REMOTE_ADDRESS, .command = CD_REMOTE_TOP_RIGHT},
      .translation =
          {
              .length = 1,
              .codeInfos =
                  {
                      {.code = {.address = TV_REMOTE_ADDRESS,
                                .command = TV_REMOTE_MUTE},
                       .delay = 0},
                  },
          },
  });
  IrTranslator.end();
}
