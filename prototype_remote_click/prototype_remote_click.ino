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
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
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
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */
#include <Arduino.h>

/*
 * Define macros for input and output pin etc.
 */
#include "PinDefinitionsAndMore.h"

//#define EXCLUDE_EXOTIC_PROTOCOLS // saves around 900 bytes program space

#include <IRremote.h>

int STATUS_PIN = LED_BUILTIN;

// On the Zero and others we switch explicitly to SerialUSB
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

void setup()
{
    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver, enable feedback LED, take LED feedback pin from the internal boards definition

    IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK); // Specify send pin and enable feedback LED at default feedback LED pin

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
    Serial.print(IR_SEND_PIN);
#endif
}

void loop()
{
    if (IrReceiver.decode())
    {
        // Print a short summary of received data
        IrReceiver.printIRResultShort(&Serial);
        if (IrReceiver.decodedIRData.protocol == UNKNOWN)
        {
            // We have an unknown protocol here, print more info
            IrReceiver.printIRResultRawFormatted(&Serial, true);
        }

        //  Finally, check the received data and perform actions according to the received command
        if (IrReceiver.decodedIRData.address == 0xEF00 && IrReceiver.decodedIRData.command == 0x1A)
        {
            Serial.println(F("Data matched! Now sending..."));
            digitalWrite(STATUS_PIN, HIGH);
            sendCodeSet();
            digitalWrite(STATUS_PIN, LOW);
        }

        // Enable receiving of the next value
        IrReceiver.resume();
    }
}

void sendCode(uint16_t sAddress,
              uint16_t sCommand)
{
    Serial.print(F("Send now: address=0x"));
    Serial.print(sAddress, HEX);
    Serial.print(F(" command=0x"));
    Serial.println(sCommand, HEX);
    IrSender.sendNEC(sAddress, sCommand, NO_REPEATS);
}

void sendCodeSet()
{
    uint16_t sAddress = 0xC7EA;
    sendCode(sAddress, 0x99);
    delay(100);
    sendCode(sAddress, 0xAD);
    delay(100);
    sendCode(sAddress, 0xAD);
    delay(100);
    sendCode(sAddress, 0xAA);
    delay(100);
    sendCode(sAddress, 0xAA);
    delay(100);
    sendCode(sAddress, 0x9E);
    delay(300);
    sendCode(sAddress, 0x99);
}
