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
// - Change MatterEndPoint to Matter AirQuality Sensor(CO2)
//
// This file is based on code from the Espressif Matter library:
// https://github.com/espressif/arduino-esp32/tree/master/libraries/Matter/src/MatterEndpoints

#include <sdkconfig.h>

#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

#include <Matter.h>
#include <app/server/Server.h>
#include "MatterAirQualitySensor.h"

using namespace esp_matter;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

MatterAirQualitySensor::MatterAirQualitySensor()
{
}

MatterAirQualitySensor::~MatterAirQualitySensor()
{
    end();
}

bool MatterAirQualitySensor::attributeChangeCB(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (!started)
    {
        log_e("Matter CO2 Sensor device has not begun.");
        return false;
    }

    log_d("CO2 Sensor Attr update callback: endpoint: %u, cluster: %u, attribute: %u, val: %u",
          endpoint_id, cluster_id, attribute_id, val->val.u32);

    return true;
}

bool MatterAirQualitySensor::begin(uint16_t _rawCO2)
{
    ArduinoMatter::_init();

    if (getEndPointId() != 0)
    {
        log_e("Matter CO2 Sensor with Endpoint Id %d already exists.", getEndPointId());
        return false;
    }

    air_quality_sensor::config_t air_quality_sensor_config;

    endpoint_t *endpoint = air_quality_sensor::create(node::get(), &air_quality_sensor_config, ENDPOINT_FLAG_NONE, (void *)this);

    if (endpoint == nullptr)
    {
        log_e("Failed to create CO2 Sensor endpoint.");
        return false;
    }

    cluster::carbon_dioxide_concentration_measurement::config_t co2_config;

    cluster_t *cluster = cluster::carbon_dioxide_concentration_measurement::create(endpoint, &co2_config, CLUSTER_FLAG_SERVER);
    if (cluster == nullptr)
    {
        log_e("Failed to create CO2 Measurement cluster.");
        return false;
    }

    uint8_t flags = ATTRIBUTE_FLAG_NULLABLE;


    attribute::create(cluster, CarbonDioxideConcentrationMeasurement::Attributes::MeasuredValue::Id, flags, esp_matter_nullable_float(static_cast<float>(_rawCO2)));
    attribute::create(cluster, CarbonDioxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id, flags, esp_matter_nullable_float(0.0f));
    attribute::create(cluster, CarbonDioxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id, flags, esp_matter_nullable_float(10000.0f));
    attribute::create(cluster, CarbonDioxideConcentrationMeasurement::Attributes::MeasurementUnit::Id, flags, esp_matter_enum8(0)); // PPM


    esp_matter_attr_val_t val;
    attribute_t *feature_map_attr = attribute::get(cluster, Globals::Attributes::FeatureMap::Id);
    if (feature_map_attr)
    {
        attribute::get_val(feature_map_attr, &val);
        val.val.u32 |= 0x1; 
        attribute::set_val(feature_map_attr, &val);
    }

    rawCO2 = _rawCO2;
    setEndPointId(endpoint::get_id(endpoint));
    log_i("CO2 Sensor created with endpoint_id %d", getEndPointId());

    cluster_t *custom_cluster = cluster::create(endpoint, AirQuality::Id, CLUSTER_FLAG_SERVER);
    if (custom_cluster == nullptr)
    {
        log_e("Failed to create custom AirQuality cluster.");
        return false;
    }

    cluster::air_quality::config_t aq_config;
    cluster_t *clusteraq = cluster::air_quality::create(endpoint, &aq_config, CLUSTER_FLAG_SERVER);
    
    attribute::create(clusteraq, AirQuality::Attributes::AirQuality::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_enum8(1)); // AirQuality = 1 (z. B. Fair)
    

    started = true;
    return true;
}

void MatterAirQualitySensor::end()
{
    started = false;
}

bool MatterAirQualitySensor::setRawCO2(uint16_t _rawCO2)
{
    if (!started)
    {
        log_e("Matter CO2 Sensor device has not begun.");
        return false;
    }

    if (rawCO2 == _rawCO2)
    {
        return true;
    }

    esp_matter_attr_val_t attrVal = esp_matter_invalid(NULL);

    if (!getAttributeVal(CarbonDioxideConcentrationMeasurement::Id, CarbonDioxideConcentrationMeasurement::Attributes::MeasuredValue::Id, &attrVal))
    {
        log_e("Failed to retrieve CO2 Sensor attribute.");
        return false;
    }

    float newValue = static_cast<float>(_rawCO2);
    attrVal = esp_matter_float(newValue);

    if (!updateAttributeVal(CarbonDioxideConcentrationMeasurement::Id, CarbonDioxideConcentrationMeasurement::Attributes::MeasuredValue::Id, &attrVal))
    {
        log_e("Failed to update CO2 Sensor attribute.");
        return false;
    }

    rawCO2 = _rawCO2;
    log_v("CO2 Sensor set to %.02f PPM", newValue);

    esp_matter_attr_val_t attrValAQ = esp_matter_invalid(NULL);
    if (!getAttributeVal(AirQuality::Id, AirQuality::Attributes::AirQuality::Id, &attrValAQ))
    {
        log_e("AirQuality attribute not found");
        return false;
    }
    int enumaq;

    if (_rawCO2 <= 800)
    {
        enumaq = 1; // Excellent
    }
    else if (_rawCO2 <= 1000)
    {
        enumaq = 2; // Good
    }
    else if (_rawCO2 <= 1500)
    {
        enumaq = 3; // Fair
    }
    else if (_rawCO2 <= 2000)
    {
        enumaq = 4; // Inferior
    }
    else
    {
        enumaq = 4; // Poor
    }

    attrValAQ = esp_matter_enum8(enumaq);
    updateAttributeVal(AirQuality::Id, AirQuality::Attributes::AirQuality::Id, &attrValAQ);
    rawaq = enumaq;
    log_v("Air Quality Sensor set to mode %d", attrValAQ);

    return true;
}

#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
