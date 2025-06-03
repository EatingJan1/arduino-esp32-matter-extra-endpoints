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

class MatterLightSensor : public MatterEndPoint, ArduinoMatter
{
public:
    MatterLightSensor();
    ~MatterLightSensor();

    // begin Matter light Sensor endpoint with initial float light percent
    bool begin(double light = 0.00)
    {
        return begin(static_cast<uint16_t>(light * 100.0f));
    }

    // stop processing light Sensor Matter events
    void end();

    // set the light percent with 1/100th of a percent precision
    bool setlight(double light)
    {

        return setRawLight(static_cast<uint16_t>(light * 100.0f));
    }


    double getlight()
    {
        return (double)rawlight / 100.0;
    }

    // assignment operator for convenience
    void operator=(double lightinmm)
    {
        setlight(lightinmm);
    }

    // cast operator
    operator double()
    {
        return (double)getlight();
    }

    // Matter attribute change callback
    bool attributeChangeCB(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val);

protected:
    bool started = false;

    uint16_t rawlight = 0;

    // internal functions
    bool begin(uint16_t _rawlight);
    bool setRawLight(uint16_t _rawlight);

};


#endif /* CONFIG_ESP_MATTER_ENABLE_DATA_MODEL */
