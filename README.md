ArduinoAirQuality
=================

Arduino Air Quality Sensor (NO2 and CO)

Using MiCS 2710 NO sensor and MiCS 5525 CO sensor to track air quality. Change in resistance in each sensor, based on ppm of respective molecule, can be measured as part of a resistive voltage divider connected to Arduino analog input.

Arduino is attached to official Arduino GSM shield. Data is sent over 3G network and Xively.com API is used to graph data on Xively website.

Further testing needs to be done to understand variance of CO sensor with temperature. NO sensor is fully functional.
