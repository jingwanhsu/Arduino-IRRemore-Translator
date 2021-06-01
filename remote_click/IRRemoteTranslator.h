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
#include <IRremote.h>

#define MAX_TRANSLATION_LENGTH 10
#define STATUS_PIN LED_BUILTIN

struct IRCode {
  uint16_t address;
  uint16_t command;
};

struct IRCodeSendInfo {
  IRCode code;
  uint32_t delay;
};

struct IRCodeSet {
  uint16_t length;
  IRCodeSendInfo codeInfos[MAX_TRANSLATION_LENGTH];
};

struct TranslateIRCode {
  IRCode original;
  IRCodeSet translation;
};

class IRTranslator {
 public:
  void begin();
  void end();
  void translate(TranslateIRCode);

 private:
  bool decoded = false;
  void sendCode(IRCodeSendInfo);
  void sendCodeSet(IRCodeSet);
};

IRTranslator IrTranslator;

void IRTranslator::begin() {
  if (!IrReceiver.decode()) {
    return;
  }
  // Print a short summary of received data
  IrReceiver.printIRResultShort(&Serial);
  if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
    // We have an unknown protocol here, print more info
    IrReceiver.printIRResultRawFormatted(&Serial, true);
  }
  decoded = true;
}

void IRTranslator::end() {
  if (!decoded) {
    return;
  }
  IrReceiver.resume();
  decoded = false;
}

void IRTranslator::translate(TranslateIRCode translateIRCode) {
  if (!decoded) {
    return;
  }
  if (IrReceiver.decodedIRData.address == translateIRCode.original.address &&
      IrReceiver.decodedIRData.command == translateIRCode.original.command) {
    Serial.println(F("Data matched! Now sending..."));
    digitalWrite(STATUS_PIN, HIGH);
    sendCodeSet(translateIRCode.translation);
    digitalWrite(STATUS_PIN, LOW);
    Serial.println(F("End sending!"));
  }
}

void IRTranslator::sendCodeSet(IRCodeSet irCodeSet) {
  for (uint16_t i = 0; i < irCodeSet.length; i++) {
    sendCode(irCodeSet.codeInfos[i]);
  }
}

void IRTranslator::sendCode(IRCodeSendInfo codeInfo) {
  uint16_t sAddress = codeInfo.code.address;
  uint16_t sCommand = codeInfo.code.command;
  Serial.print(F("Send now: address=0x"));
  Serial.print(sAddress, HEX);
  Serial.print(F(" command=0x"));
  Serial.println(sCommand, HEX);
  IrSender.sendNEC(sAddress, sCommand, NO_REPEATS);
  delay(codeInfo.delay);
}