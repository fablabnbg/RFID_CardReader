# RFID_CardReader

A device to read member cards and activate peripheral devices.

## Documentation / Requirements
- See [doc/](doc/) subdirectory for requirements, concepts and schematics

## Software

### ESP
- Example software in subdirectory [Software_ESP](Software_ESP)
- To be built with platformio
- Use "Automaton" library for state machine and WiFi / HTTP connectivity
- Continious integration on travis-ci: [![Build Status](https://api.travis-ci.org/euphi/RFID_CardReader.svg?branch=master)](https://travis-ci.org/euphi/RFID_CardReader)
- For details see [Software_ESP/README.md](Software_ESP/README.md)


### Webserver
 - not yet implemented
 - planning: Use simple server on Raspberry PI
