# Arduino-IRRemore-Translator
Translate incoming IR signal to a set of outgoing IR signals.

## Prerequisites

It's based on [Arduino-IRremote](https://github.com/Arduino-IRremote/Arduino-IRremote), please make sure you have the library installed.

## How to use

Please see the example `remote_click/remote_click.ino`. It's translating signals of a CD player remote to of the TV remote. The translator library is `IRRemoteTranslator.h`. You can define your own translations by including that in your own design.