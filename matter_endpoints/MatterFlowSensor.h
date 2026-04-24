// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

class MatterFlowSensor : public MatterEndPoint, public ArduinoMatter
{
public:
  MatterFlowSensor();
  ~MatterFlowSensor();
  // begin Matter Flow Sensor endpoint with initial float Flow
  bool begin(double Flow = 0.00) {

    return begin(static_cast<int16_t>(Flow * 10.0f));
  }
  void end();

  // set the Flow
  bool setFlow(double Flow) {
    return setRawFlow(static_cast<int16_t>(Flow * 10.0f));
  }
  // returns the reported float Flow
  double getFlow() {
    return (double)rawFlow / 10.0;
  }
  // double conversion operator
  void operator=(double Flow) {
    setFlow(Flow);
  }
  // double conversion operator
  operator double() {
    return (double)getFlow();
  }

  // this function is called by Matter internal event processor. It could be overwritten by the application, if necessary.
  bool attributeChangeCB(int16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val);

protected:
  bool started = false;
  // implementation keeps Flow relative percentage with 1/100th of a percent precision
  int16_t rawFlow = 0;
  // internal function to set the raw Flow value (Matter Cluster)
  bool begin(int16_t _rawFlow);
  bool setRawFlow(int16_t _rawFlow);
};
#endif /* CONFIG_ESP_MATTER_ENABLE_DATA_MODEL */