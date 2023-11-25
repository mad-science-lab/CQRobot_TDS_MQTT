/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "MyWIFI"
#define WLAN_PASS       "SecurePassword123"

/***************************** MQTT Setup ************************************/

#define MQTT_SERVER      "homeassistant.local"       //MQTT Broker host name (mqtt.domain.com) or ip
#define MQTT_SERVERPORT  1883                        // use 8883 for SSL
#define MQTT_USERNAME    "MQTTUser"
#define MQTT_PASSWORD    "MQTTPassword"
#define UNIQUE_ID        "SensorName"                //user defined id, useful if you have many sensor packs
