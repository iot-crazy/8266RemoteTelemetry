# 8266RemoteTelemetry

Remote telemetry send from an 8266 board using WiFi

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.


### Prerequisites

You will need:
1. An ESP9266 board with an available I2C connection
2. GY-21 sensor
3. BME/BMP280 sensor
4. Connecting wires

Either sensor is optional if you do not require telemetry of that type. Remember to remove the code for the sensor you do not connect

You will already know:
1. How to use the Arduino IDE (or IDE of your choice)
2. How to upload sketches / code to your device

### Installing

1. Clone the repo to your local computer, or copy the .ino file.
2. Connect the sensors to your ESP8266 board
3. Set the WiFi network name and password in the code (around lines 18 & 19)
4. Connect the sensors to 3.3V and Ground. The SCL pin from both sensors to pin D1 on the NodeMCU, SDA to pin D2, or the corresponding SCL & SDA pins for your device.

Debug and telemetry data are also output to the serial port to allow testing without the client device.

## Tested On

1. NodeeMCU ESP-12E development board

Other baords coming soone
Please let us know if you test this on other devices so we can add to the list and acknowledge your contribution!

Does not work on:
1. Heltec Wifi Kit 32 due to the onboard OLED display taking over the I2C connections

## Trouble Shooting

### A sensor will not initialise
1. The ID number returned by some sensors does not always match the ID expected by the library. We found this on the BME280. Finding the ID requires editing the library, although the device manufacturer should be able to tell you this also.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
