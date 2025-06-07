// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sdkconfig.h>
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

#include <Matter.h>
#include <app/server/Server.h>
#include "MatterFlowSensor.h"

using namespace esp_matter;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

bool MatterFlowSensor::attributeChangeCB(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val) {
  bool ret = true;
  if (!started) {
    log_e("Matter Flow Sensor device has not begun.");
    return false;
  }

  log_d("Flow Sensor Attr update callback: endpoint: %u, cluster: %u, attribute: %u, val: %u", endpoint_id, cluster_id, attribute_id, val->val.u32);
  return ret;
}

MatterFlowSensor::MatterFlowSensor() {}

MatterFlowSensor::~MatterFlowSensor() {
  end();
}

bool MatterFlowSensor::begin(uint16_t _rawFlow) {
  ArduinoMatter::_init();

  if (getEndPointId() != 0) {
    log_e("Matter Flow Sensor with Endpoint Id %d device has already been created.", getEndPointId());
    return false;
  }



  flow_sensor::config_t flow_sensor_config;
  flow_sensor_config.flow_measurement.flow_measured_value = _rawFlow;
  flow_sensor_config.flow_measurement.flow_min_measured_value = nullptr;
  flow_sensor_config.flow_measurement.flow_max_measured_value = nullptr;

  // endpoint handles can be used to add/modify clusters.
  endpoint_t *endpoint = flow_sensor::create(node::get(), &flow_sensor_config, ENDPOINT_FLAG_NONE, (void *)this);
  if (endpoint == nullptr) {
    log_e("Failed to create Flow Sensor endpoint");
    return false;
  }
  rawFlow = _rawFlow;
  setEndPointId(endpoint::get_id(endpoint));
  log_i("Flow Sensor created with endpoint_id %d", getEndPointId());
  started = true;
  return true;
}

void MatterFlowSensor::end() {
  started = false;
}

bool MatterFlowSensor::setRawFlow(uint16_t _rawFlow) {
  if (!started) {
    log_e("Matter Flow Sensor device has not begun.");
    return false;
  }

  // avoid processing if there was no change
  if (rawFlow == _rawFlow) {
    return true;
  }

  esp_matter_attr_val_t humidityVal = esp_matter_invalid(NULL);

  if (!getAttributeVal(FlowMeasurement::Id, FlowMeasurement::Attributes::MeasuredValue::Id, &humidityVal)) {
    log_e("Failed to get Flow Sensor Attribute.");
    return false;
  }
  if (humidityVal.val.u16 != _rawFlow) {
    humidityVal.val.u16 = _rawFlow;
    bool ret;
    ret = updateAttributeVal(FlowMeasurement::Id, FlowMeasurement::Attributes::MeasuredValue::Id, &humidityVal);
    if (!ret) {
      log_e("Failed to update Flow Sensor Attribute.");
      return false;
    }
    rawFlow = _rawFlow;
  }
  log_v("Flow Sensor set to %.02f Percent", (float)_rawFlow / 100.00);

  return true;
}

#endif /* CONFIG_ESP_MATTER_ENABLE_DATA_MODEL */