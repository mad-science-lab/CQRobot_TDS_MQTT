# Integrating CQRobot TDS Meter Sensor with Home Assistant via MQTT

## Overview
The CQRobot TDS Meter Sensor measures Total Dissolved Solids in water and is compatible with Arduino. This guide explains how to integrate it with Home Assistant using MQTT for monitoring water quality.

## Specifications
- **Input Voltage:** 3.3V to 5.5V.
- **Output Voltage:** 0 to 2.3V.
- **TDS Measurement Range:** 0 to 1000ppm.
- **Accuracy:** ±10% F.S. at 25°C.
- **Module Size:** 43mm x 32.2mm.
- **Probe:** Waterproof, 83cm length.

## Precautions
- Avoid using the probe in water above 55°C.
- Keep the probe away from container edges.
- Note that the connector and the signal transmitter board are not waterproof.

## Steps
1. **Sensor Connection:** Connect the TDS Meter Sensor to an Arduino or a similar controller.
2. **Upload Arduino Scketch:** This program for an ESP8266 will read TDS values from the sensor and publish the data to a MQTT Broker. The scketch buffers the by only sending every X data packet, the get TDS is averaging readings and we don't need that much MQTT Traffic. 
3. **Home Assistant Configuration:**
   - Ensure Home Assistant has MQTT integration enabled.
   - Configure MQTT sensors in Home Assistant to receive TDS data from the MQTT broker.
4. **Monitoring:** Use Home Assistant's dashboard to display real-time TDS readings.

## Applications
- **Hydroponics:** Monitor nutrient levels in water.
- **Domestic Water:** Assess water quality.


## Example configuration.yaml entry
```
mqtt:
  sensor:
    - name: "TDS"
      state_topic: "TDSsensor/data/TDS"
```
