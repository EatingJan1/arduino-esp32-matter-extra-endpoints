#include <Matter.h>
#include <WiFi.h>
#include <scr/MatterAirQualitySensor.h>
#include <scr/MatterAirQualitySensor.h>
#include <scr/MatterFlowSensor.h>
#include <scr/MatterLightSensor.h>
#include <scr/MatterRainSensorSensor.h>

// Custom Matter Components
MatterAirQualitySensor airSensor;
MatterAirQualitySensor co2;
MatterRainSensor rainSensor;
MatterLightSensor lightSensor;
MatterFlowSensor flowSensor;

const char *ssid = "your-ssid";
const char *password = "your-password";
const uint8_t buttonPin = BOOT_PIN;

uint32_t button_time_stamp = 0;
bool button_state = false;
const uint32_t decommissioningTimeout = 5000;

// Simulated Sensor Values
float getSimulatedCO2()
{
    static float co2 = 400.0;
    co2 += 50;
    if (co2 > 2000.0)
    {
        co2 = 400.0;
    }
    return co2;
}

bool getSimulatedRain()
{
    static bool rain = false;
    rain = !rain;
    return rain;
}

float getSimulatedLight()
{
    static float lux = 100.0;
    lux += 50;
    if (lux > 1000.0)
    {
        lux = 100.0;
    }
    return lux;
}

float getSimulatedFlow()
{
    static float flow = 0.0;
    flow += 0.5;
    if (flow > 10.0)
    {
        flow = 0.0;
    }
    return flow;
}

void setup()
{
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    // Initialize all custom endpoints
    airSensor.begin(getSimulatedCO2());
    rainSensor.begin(getSimulatedRain());
    lightSensor.begin(getSimulatedLight());
    flowSensor.begin(getSimulatedFlow());

    // Start Matter stack after all endpoints
    Matter.begin();

    // Commissioning check
    if (!Matter.isDeviceCommissioned())
    {
        Serial.println("Matter Node is not yet commissioned.");
        Serial.printf("Manual pairing code: %s\r\n", Matter.getManualPairingCode().c_str());
        Serial.printf("QR code URL: %s\r\n", Matter.getOnboardingQRCodeUrl().c_str());

        while (!Matter.isDeviceCommissioned())
        {
            delay(100);
            Serial.println("Waiting for commissioning...");
        }

        Serial.println("Matter Node successfully commissioned.");
    }
}

void loop()
{
    static uint32_t counter = 0;

    if (!(counter++ % 10))
    {
        float co2 = getSimulatedCO2();
        airSensor.setCO2(co2);
        Serial.printf("CO₂: %.1f ppm\r\n", co2);

        bool rain = getSimulatedRain();
        rainSensor.setRain(rain);
        Serial.printf("Rain: %s\r\n", rain ? "yes" : "no");

        float lux = getSimulatedLight();
        lightSensor.setlight(lux);
        Serial.printf("Light: %.1f Lux\r\n", lux);

        float flow = getSimulatedFlow();
        flowSensor.setFlow(flow);
        Serial.printf("Flow: %.2f L/min\r\n", flow);
    }

    // Button debounce and long-press for decommissioning
    if (digitalRead(buttonPin) == LOW && !button_state)
    {
        button_time_stamp = millis();
        button_state = true;
    }

    if (digitalRead(buttonPin) == HIGH && button_state)
    {
        button_state = false;
    }

    if (button_state && (millis() - button_time_stamp) > decommissioningTimeout)
    {
        Serial.println("Decommissioning Matter Node.");
        Matter.decommission();
    }

    delay(500);
}