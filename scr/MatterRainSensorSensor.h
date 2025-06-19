// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Modifications 2025 by Blue Rubber Duck
// - Change MatterEndPoint to Matter illuminance Sensor
//
// This file is based on code from the Espressif Matter library:
// https://github.com/espressif/arduino-esp32/tree/master/libraries/Matter/src/MatterEndpoints

#pragma once
#include <sdkconfig.h>
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

#include <Matter.h>
#include <MatterEndPoint.h>

class MatterRainSensor : public MatterEndPoint, public ArduinoMatter
{
public:
  MatterRainSensor();
  ~MatterRainSensor();
  // begin Matter Rain Sensor endpoint with initial contact state
  bool begin(bool _RainState = false);
  // this will just stop processing Rain Sensor Matter events
  void end();

  // set the contact state
  bool setRain(bool _RainState);
  // returns the contact state
  bool getRain() {
    return RainState;
  }

  // bool conversion operator
  void operator=(bool _RainState) {
    setRain(_RainState);
  }
  // bool conversion operator
  operator bool() {
    return getRain();
  }

  // this function is called by Matter internal event processor. It could be overwritten by the application, if necessary.
  bool attributeChangeCB(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val);

protected:
  bool started = false;
  bool RainState = false;
};
#endif /* CONFIG_ESP_MATTER_ENABLE_DATA_MODEL */