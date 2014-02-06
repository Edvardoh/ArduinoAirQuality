/*
 Arduino AirQuality Tracker - NO2 in PartsPerMillion (PPM)
 
 Data transmited to Xively account using Telefonica GSM/GPRS Shield.
 
Code from the following public domain has been modified and built upon: 
 http://arduino.cc/en/Tutorial/GSMExamplesXivelyClient
 
 */

// libraries
#include <GSM.h>

// Xively Client data
#define APIKEY         "wG2OO23rxPuFjIXy3PjuHmWYnyVdnPlW0jHLkGTDUDOqv2pl"  // replace your xively api key here
#define FEEDID         550274806                     // replace your feed ID
#define USERAGENT      "Air Quality Tracker"              // user agent is the project name

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "bluevia.movistar.es"  // replace your GPRS APN
#define GPRS_LOGIN     ""     // replace with your GPRS login
#define GPRS_PASSWORD  ""  // replace with your GPRS password

// initialize the library instance:
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// IPAddress server(216,52,233,121);    // numeric IP for api.xively.com
char server[] = "api.xively.com";      // name address for xively API

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  //delay between updates to Xively.com

//NO2 sensor variables
const int NO2=3; //NO2 sensor on analog 3
float ADCreading = 0; //variable to hold ADC reading
float Vout = 0; //output voltage
float Rs = 0; //Sensor Resistance
float ppbNO2 = 0; //ppb NO2

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("Starting debugging");
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  */
  // connection state
  boolean notConnected = true;
  
  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
    Serial.println("in while loop");
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      {Serial.println("in nested while-if");
      notConnected = false;
      }
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
}

void loop()
{  
  // read the  sensor:
  ADCreading = analogRead(NO2);   
  Vout = ADCreading/409.2;  //convert from ADC value back to voltage
  Rs = 22000/((5/Vout) - 1);   // find sensor resistance from Vout, using 5V input & 22kOhm load resistor
  ppbNO2 = (.000008*Rs - .0194)*1000;    //convert Rs to ppm concentration NO2 (equation derived from data found on http://airpi.es/sensors.php
  Serial.print("PPB NO2= ");
  Serial.println(ppbNO2);
  delay(1000);
  float sensorReading = ppbNO2;

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available())
  {
     char c = client.read();
     Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected)
  {
    client.stop();
  }
  
  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && ((millis() - lastConnectionTime) > postingInterval))
  {
  sendData(sensorReading);
  }
  
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

/*
  This method makes a HTTP connection to the server.
*/
void sendData(int thisData)
{
  // if there's a successful connection:
  if (client.connect(server, 80))
  {
    Serial.println("connecting...");
    
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.xively.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 8 + getLength(thisData);
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();
    
    // here's the actual content of the PUT request:
    client.print("sensor1,");
    client.println(thisData);
  } 
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted
  lastConnectionTime = millis();
}

/*
  This method calculates the number of digits in the
  sensor reading.  Since each digit of the ASCII decimal
  representation is a byte, the number of digits equals
  the number of bytes.
*/
int getLength(int someValue)
{
  // there's at least one byte:
  int digits = 1;
  
  // continually divide the value by ten, 
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;
  while (dividend > 0)
  {
    dividend = dividend /10;
    digits++;
  }
  
  // return the number of digits:
  return digits;
}

