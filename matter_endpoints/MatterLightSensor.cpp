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

#include <sdkconfig.h>

#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

#include <Matter.h>
#include <app/server/Server.h>
#include "MatterLightSensor.h"

using namespace esp_matter;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

MatterLightSensor::MatterLightSensor() {}

MatterLightSensor::~MatterLightSensor() 
{
    end();
}


bool MatterLightSensor::attributeChangeCB(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val) 
{
    if (!started) 
    {
        log_e("Matter Light Sensor device has not begun.");
        return false;
    }

    log_d("Light Sensor Attr update callback: endpoint: %u, cluster: %u, attribute: %u, val: %u",
          endpoint_id, cluster_id, attribute_id, val->val.u32);

    return true;
}

bool MatterLightSensor::begin(uint16_t _rawLight) 
{
    Serial.print("1");
    ArduinoMatter::_init();
    Serial.print("2");
    if (getEndPointId() != 0) 
    {
        log_e("Matter Light Sensor with Endpoint Id %d already exists.", getEndPointId());
        return false;
    }


  light_sensor::config_t light_sensor_config;
  light_sensor_config.illuminance_measurement.illuminance_measured_value = _rawLight;
  light_sensor_config.illuminance_measurement.illuminance_min_measured_value = nullptr;
  light_sensor_config.illuminance_measurement.illuminance_max_measured_value = nullptr;

  Serial.print("3");
    endpoint_t *endpoint = light_sensor::create(node::get(), &light_sensor_config, ENDPOINT_FLAG_NONE, (void *)this);
    if (endpoint == nullptr) 
    {
        log_e("Failed to create Light Sensor endpoint.");
        return false;
    }
    Serial.print("4");
    rawlight = _rawLight;
    setEndPointId(endpoint::get_id(endpoint));
    log_i("Light Sensor created with endpoint_id %d", getEndPointId());

    started = true;
    Serial.print("5");
    return true;
}

void MatterLightSensor::end() 
{
    started = false;
}

bool MatterLightSensor::setRawLight(uint16_t _rawLight) 


{
    if (!started) 
    {
        log_e("Matter Light Sensor device has not begun.");
        return false;
    }


    if (rawlight == _rawLight) 
    {
        return true;
    }

    esp_matter_attr_val_t attrVal = esp_matter_invalid(NULL);

    if (!getAttributeVal(IlluminanceMeasurement::Id, IlluminanceMeasurement::Attributes::MeasuredValue::Id, &attrVal)) 
    {
        log_e("Failed to retrieve Light Sensor attribute.");
        return false;
    }

    if (attrVal.val.u16 != _rawLight) 
    {
        attrVal.val.u16 = _rawLight;

        if (!updateAttributeVal(IlluminanceMeasurement::Id, IlluminanceMeasurement::Attributes::MeasuredValue::Id, &attrVal)) 
        {
            log_e("Failed to update Light Sensor attribute.");
            return false;
        }

        rawlight = _rawLight;
    }

    log_v("Light Sensor set to %.02f%%", (float)_rawLight / 100.0);
    return true;
}

#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

