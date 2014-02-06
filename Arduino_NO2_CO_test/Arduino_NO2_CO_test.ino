//NO2 sensor testing

const int NO2=3; //NO2 sensor on analog 3
const int CO=2; //CO sensor on analog 2
float Vout = 0; //output voltage
float Rs = 0; //Sensor Resistance
float ppbNO2 = 0; //ppb NO2
float ppmCO = 0; //ppm CO

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("Starting debugging");
}

void loop()
{  
  // read NO2 sensor:
  Vout = analogRead(NO2)/409.2; // take reading and convert ADC value to voltage
  Rs = 22000/((5/Vout) - 1);   // find sensor resistance from Vout, using 5V input & 22kOhm load resistor
  ppbNO2 = (.000008*Rs - .0194)*1000;    //convert Rs to ppb concentration NO2 (equation derived from data found on http://airpi.es/sensors.php
  
  Serial.print("PPB NO2= ");
  Serial.println(ppbNO2);

  // read CO sensor:
  Vout = analogRead(CO)/409.2; // take reading and convert ADC value to voltage
  Rs = 100000/((5/Vout) - 1);   // find sensor resistance from Vout, using 5V input & 100kOhm load resistor
  ppmCO = 911.19*pow(2.71828,(-8.577*Rs/100000));    //convert Rs to ppm concentration CO
  
  Serial.print("PPM CO= ");
  Serial.println(ppmCO);

  delay(1000);
}

