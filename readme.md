# Matter Extra Endpoints for Arduino-ESP32

This repository extends the official [arduino-esp32](https://github.com/espressif/arduino-esp32) Matter library with additional custom endpoints for environmental sensing, designed for integration into projects such as a connected weather station.

## Overview

These extensions build on top of the `Matter.h` interface provided by the `arduino-esp32/libraries/Matter` module. This wrapper offers a simplified and Arduino-friendly API for the Matter protocol stack, which itself is based on the more advanced [esp-matter](https://github.com/espressif/esp-matter) library by Espressif.

With these extensions, developers can easily integrate new sensor types into their Matter-enabled ESP32 devices using a consistent API model (`begin()`, `setX()`, `onChangeX()`).

## Added Components

This project includes four custom Matter endpoint classes:

* **MatterAirQualitySensor**

  * Supports CO₂ measurement (ppm).
  * Note: currently limited to CO₂ only, no full air quality index support yet.

* **MatterRainSensor**

  * Detects whether rain is present (boolean value).

* **MatterAmbientLightSensor**

  * Measures illuminance in lux.

* **MatterFlowSensor**

  * Measures flow rate (liters per minute or similar).

Each component class encapsulates a Matter endpoint and exposes methods to initialize and update its state.

## Repository Structure

* `src/`

  * `MatterAirQualitySensor.h/cpp`
  * `MatterRainSensor.h/cpp`
  * `MatterAmbientLightSensor.h/cpp`
  * `MatterFlowSensor.h/cpp`

## Requirements

* ESP32 board
* [arduino-esp32](https://github.com/espressif/arduino-esp32) (with Matter support)

## License

This project is licensed under the Apache License 2.0.

## Author

Developed by EatingJan1

---

Feedback and contributions are welcome!
