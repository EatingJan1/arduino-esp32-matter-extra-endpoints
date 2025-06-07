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

#pragma once
#include <sdkconfig.h>
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

#include <Matter.h>
#include <MatterEndPoint.h>

class MatterAirQualitySensor : public MatterEndPoint, ArduinoMatter
{
public:
    MatterAirQualitySensor();
    ~MatterAirQualitySensor();

    bool begin(double CO2 = 0.00)
    {
        return begin(static_cast<uint16_t>(CO2));
    }

    void end();

    bool setCO2(double CO2)
    {

        return setRawCO2(static_cast<uint16_t>(CO2));
    }


    double getCO2()
    {
        return (double)rawCO2;
    }
    
    void operator=(double ppm)
    {
        setCO2(ppm);
    }
    
    operator double()
    {
        return (double)getCO2();
    }
    
    bool attributeChangeCB(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val);
    
    double getAirQuality()
    {
        return (double)rawaq;
    }
    
    protected:
    bool started = false;

    uint16_t rawCO2 = 0;
    uint8_t rawaq = 0;


    // internal functions
    bool begin(uint16_t _rawCO2);
    bool setRawCO2(uint16_t _rawCO2);

};


#endif /* CONFIG_ESP_MATTER_ENABLE_DATA_MODEL */
