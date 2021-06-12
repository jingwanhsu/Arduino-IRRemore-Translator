/*
 * IRRemoteTranslator.h
 *
 * Translate incoming IR signal to a set of outgoing IR signals
 * The logic is:
 * Define the original signal you want to translate
 * Define the set of translated signals with delay in between
 *
 * An example is remote_click.ino which translate the signals of
 * CD player remote to of TV remote.
 *
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
  char* name;
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
    Serial.print(F("Received data matches "));
    Serial.println(translateIRCode.name);
    Serial.println(F("Start sending..."));
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