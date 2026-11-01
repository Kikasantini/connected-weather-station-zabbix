# Connected Weather Station with MicroPython Display and Zabbix

An academic Internet of Things project built for the Connected Objects Programming course at Institut Grasset (2022). Two ESP8266 boards communicate over a local network: an Arduino/C++ board reads a BME280 sensor and controls two LEDs, while a MicroPython board requests the readings as JSON and displays them on an SSD1306 OLED. Zabbix monitors the values exposed by the Arduino web server.

## How it works

- The Arduino ESP8266 reads temperature, humidity, pressure, and estimated altitude from a BME280.
- Two push buttons select which LED is controlled and toggle its state.
- The Arduino board exposes sensor and LED data through `/meteoJSON` and `/meteoHTML` HTTP endpoints.
- The MicroPython ESP8266 fetches `/meteoJSON`, displays the readings and LED states on an SSD1306 OLED, and uses a potentiometer to adjust display contrast and turn the display off at a low setting.
- Zabbix monitors the data served by the Arduino board.

## Repository layout

- `firmware/arduino-weather-server/` — Arduino/C++ firmware and a safe Wi-Fi configuration template.
- `firmware/micropython-display/` — MicroPython client code. The `SSD1306.py` driver must also be installed on the MicroPython board.
- `media/` — wiring diagram and the full project demonstration video.

## Hardware

- Two ESP8266-compatible development boards (one running Arduino/C++, one running MicroPython)
- BME280 temperature, humidity, and pressure sensor
- SSD1306 128×64 I²C OLED display
- Two LEDs and current-limiting resistors
- Two push buttons
- Potentiometer

See [`media/wiring-diagram.png`](media/wiring-diagram.png) for the circuit diagram.

## Arduino libraries

Install the libraries used by the Arduino sketch through the Arduino IDE Library Manager: `PinButton`, `Adafruit Unified Sensor`, and `Adafruit BME280 Library`. Select an ESP8266 Arduino board package compatible with your board. The sketch also uses the ESP8266 Wi-Fi, web-server, mDNS, and Wire libraries supplied by that board package.

## Setup

1. Open `firmware/arduino-weather-server/WeatherStationServer.ino` in the Arduino IDE.
2. Copy `MyWifi.example.h` to `MyWifi.h` in the same folder and enter the credentials for your own test Wi-Fi network. `MyWifi.h` is excluded from Git.
3. Upload the sketch and note the ESP8266 server's local IP address in the serial monitor.
4. On the MicroPython board, install the SSD1306 driver and copy `Partie2.py` and `main.py` to the device.
5. Update the `HOST` value in `Partie2.py` to the Arduino board's local IP address, then run `main.py`. Both boards must be on the same network.
6. Configure Zabbix items to retrieve the required values from the Arduino board's `/meteoJSON` endpoint.

## Demonstration videos

- [Full project demonstration](media/full-project-demo.mp4)

## Security note

The original archive contained a Wi-Fi configuration file and a local IP address. The real credentials were not copied into this repository; use the example file and your own network settings.
