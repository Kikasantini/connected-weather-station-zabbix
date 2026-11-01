# Zabbix monitoring notes

The project uses Zabbix to monitor values exposed by the Arduino/C++ weather-station server. The source notes identify these measurements and application values:

- Temperature, humidity, pressure, and estimated altitude from the BME280
- Selected light index (`0` or `1`)
- State of each LED (`ON` or `OFF`)

The MicroPython board consumes the same JSON response to update the SSD1306 display. The original class-specific Zabbix configuration and screenshots were not included in the archive, so the exact item keys and dashboard setup need to be recreated for the target Zabbix instance.
