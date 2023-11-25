// http://www.cqrobot.wiki/index.php/TDS_(Total_Dissolved_Solids)_Meter_Sensor_SKU:_CQRSENTDS01

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "CONFIG.h"

#define TdsSensorPin A0
#define VREF 3.3      // analog reference voltage(Volt) of the ADC
#define MYSCOUNT 30           // sum of sample point

int analogBuffer[MYSCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[MYSCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;
int CountA = 1;
int CountReset = 60;

WiFiClient client;
/*****************************MQTT  Feeds ************************************/

Adafruit_MQTT_Client mqtt(&client,MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

// Setup a feeds for publishing to the MQTT Server
Adafruit_MQTT_Publish statusmessage = Adafruit_MQTT_Publish(&mqtt, UNIQUE_ID "/tele/INFO1");
Adafruit_MQTT_Publish teleINFO1 = Adafruit_MQTT_Publish(&mqtt, UNIQUE_ID "/tele/INFO2");
Adafruit_MQTT_Publish tdsdata = Adafruit_MQTT_Publish(&mqtt, UNIQUE_ID "/data/TDS");     //ADD

const char *chrLocalIp;

void connectWifi(){

// Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  delay(1500);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  delay(2000);
  Serial.println("Hello World!");  
  pinMode(TdsSensorPin, INPUT); //ADD

  connectWifi();                                       
  MQTT_connect();  // Connect to the MQTT Broker

  statusmessage.publish("Alive!");
  String strLocalIP = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
  chrLocalIp = strLocalIP.c_str();
  teleINFO1.publish(chrLocalIp);
}

void loop() {
  delay(100);
  MQTT_connect();                               //Make sure we are connected 
  if(! mqtt.ping()) {mqtt.disconnect();}        // ping MQTT Broker to keep alive
  getTDS();   //ADD
}

int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

void getTDS()
{
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == MYSCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U)
  {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < MYSCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, MYSCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge = averageVoltage / compensationCoefficient; //temperature compensation
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; //convert voltage value to tds value
    //Serial.print("voltage:");
    //Serial.print(averageVoltage,2);
    //Serial.print("V   ");
    Serial.print(CountA);
    Serial.print(" TDS----Value:");
    Serial.print(tdsValue, 0);
    Serial.println("ppm");

    if (CountA > (CountReset - 1)){
        tdsdata.publish(tdsValue);
        CountA = 1;
        } else {
        CountA = CountA + 1;
      }

  }
}
