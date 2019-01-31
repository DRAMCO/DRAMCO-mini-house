/*  ____  ____      _    __  __  ____ ___
 * |  _ \|  _ \    / \  |  \/  |/ ___/ _ \
 * | | | | |_) |  / _ \ | |\/| | |  | | | |
 * | |_| |  _ <  / ___ \| |  | | |__| |_| |
 * |____/|_| \_\/_/   \_\_|  |_|\____\___/
 *                           research group
 *                             dramco.be/
 *
 *  KU Leuven - Technology Campus Gent,
 *  Gebroeders De Smetstraat 1,
 *  B-9000 Gent, Belgium
 *
 *         File: main.cpp
 *      Created: 2019-01-25
 *       Author: Geoffrey Ottoy
 *
 *  Description: MQTT IoT Device
 *
 *  License: GNU GPL v3.0 (see LICENSE)
 */

#include <Arduino.h>
#include <Wire.h>    // I2C library

#include "settings.h"
#include "I2Cbus.h"
// Sensor-specific includes
#if defined(DEVICE_TYPE)
  #if (DEVICE_TYPE == TYPE_COLOR_SENSOR)
    #include "ColorSensor.h"
    #define PERIODIC_UPDATES
  #elif (DEVICE_TYPE == TYPE_CO2_SENSOR)
    #include "CO2Sensor.h"
    #define PERIODIC_UPDATES
  #elif (DEVICE_TYPE == TYPE_FIRE_PLACE)
    #include "FirePlace.h"
    #define IS_SUBSCRIBER
  #elif (DEVICE_TYPE == TYPE_DOOR_LOCK)
    #include "DoorLock.h"
    #define IS_SUBSCRIBER
  #elif (DEVICE_TYPE == TYPE_LUX_SENSOR)
    #include "VEML7700.h"
    #define PERIODIC_UPDATES
  #elif (DEVICE_TYPE == TYPE_CURTAINS)
    #include "Solscreen.h"
    #define IS_SUBSCRIBER
  #elif (DEVICE_TYPE == TYPE_BUTTON)
    #define BUTTON_PIN 16
  #else
   #error "DEVICE_TYPE not supported"
  #endif
#else
 #error "DEVICE_TYPE not defined"
#endif

// Networking libraries
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "mDNSResolver.h"
using namespace mDNSResolver;

#define COLOR_TRESHOLD 0x40

// Networking
WiFiUDP udp;
Resolver resolver(udp);
IPAddress mqtt_server;
WiFiClient espClient;
PubSubClient client(espClient);
// mqtt messaging
long lastMsg = 0;
char message_status[MAX_MESSAGE_LEN];
char message_set[MAX_MESSAGE_LEN];
char topic_status[MAX_TOPIC_LEN];
char topic_set[MAX_TOPIC_LEN];
char topic_set_all[MAX_TOPIC_LEN];
bool set_received = false;

// Sensors and actuators
#if (DEVICE_TYPE == TYPE_COLOR_SENSOR)
  ColorSensor colorSensor;
#endif
#if (DEVICE_TYPE == TYPE_CO2_SENSOR)
  CO2Sensor co2Sensor;
#endif
#if (DEVICE_TYPE == TYPE_FIRE_PLACE)
  FirePlace firePlace;
#endif
#if (DEVICE_TYPE == TYPE_DOOR_LOCK)
  DoorLock doorLock;
#endif
#if (DEVICE_TYPE == TYPE_LUX_SENSOR)
  VEML7700 als;
#endif
#if (DEVICE_TYPE == TYPE_CURTAINS)
  Solscreen curtains;

  bool event = false;

  void cb(void){
    event = true;
  }
#endif

unsigned long lastMeasurement = 0;

#if (DEVICE_TYPE == TYPE_BUTTON)
bool btn_pressed = false;

void btn_callback(void){
  btn_pressed = true;
}
#endif

/* Setup networking en connect to mqtt server
 */
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  resolver.setLocalIP(WiFi.localIP());
  mqtt_server = resolver.search(mqtt_name);
  Serial.print("MQTT server found at: "); Serial.println(mqtt_server);
}

/* Attemt to reconnect to the mqtt server
 */
void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...

      // ... and (re)subscribe
#ifdef IS_SUBSCRIBER
      client.subscribe(topic_set);
      client.subscribe(topic_set_all);
#endif
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/* Callback for receiving mqtt message_statuss
 */
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("message_status received [");
  Serial.print(topic);
  Serial.print("] ");
  memset(message_set, '\0', MAX_MESSAGE_LEN);
  for(unsigned int i = 0; i < length; i++){
    Serial.print((char)payload[i]);

    if(i<MAX_MESSAGE_LEN){
      message_set[i] = (char)payload[i];
    }
  }
  Serial.println();
  // set flag
  set_received = true;
}

/* Main setup
 */
void setup() {
  // Init serial
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // I2C bus clear (advisable)
  Serial.print("Clearing I2C bus: ");
  Serial.println(I2Cbus_statusstr(I2Cbus_clear(SDA,SCL)));

#if (DEVICE_TYPE == TYPE_COLOR_SENSOR)
  // Init RGB Color Sensor
  // ! This needs to be done first because it internally initializes Wire !
  Serial.println("Init ColorSensor...");
  if(colorSensor.begin()){
    Serial.println("   OK");
  }
  else{
    Serial.println("   FAILED");
  }
  sprintf_P(topic_status, "colorsensor/%s/status", sensorid);
  Serial.println("Done.");
#endif

#if (DEVICE_TYPE == TYPE_CO2_SENSOR)
  // Init CO2 Sensor
  Serial.println("Init CO2 Sensor... ");
  if(co2Sensor.begin()){
    Serial.println("   OK");
  }
  else{
    Serial.println("   FAILED");
  }
  sprintf_P(topic_status, "co2sensor/%s/status", sensorid);
  Serial.println("Done.");
#endif

#if (DEVICE_TYPE == TYPE_LUX_SENSOR)
  // Init Ambient light sensor
  Serial.println("Init Ambient Light Sensor...");
  als.begin();
  sprintf_P(topic_status, "luxsensor/%s/status", sensorid);
  Serial.println("Done.");
#endif

#if (DEVICE_TYPE == TYPE_FIRE_PLACE)
  // Get a fire going in the Fireplace (runs in the background).
  // Color can be changed using setFireColor(rgb).
  Serial.println("Init FirePlace...");
  firePlace.begin();
  sprintf_P(topic_status, "fireplace/%s/status", sensorid);
  sprintf_P(topic_set, "fireplace/%s/set", sensorid);
  sprintf_P(topic_set_all, "fireplace/all/set");
  Serial.println("Done.");
#endif

#if (DEVICE_TYPE == TYPE_DOOR_LOCK)
  // Init Door Lock
  Serial.println("Init DoorLock...");
  doorLock.begin();
  sprintf_P(topic_status, "doorlock/%s/status", sensorid);
  sprintf_P(topic_set, "doorlock/%s/set", sensorid);
  sprintf_P(topic_set_all, "doorlock/all/set");
  Serial.println("Done.");
#endif

#if (DEVICE_TYPE == TYPE_CURTAINS)
  // Init solscreen
  Serial.println("Init Solscreen...");
  curtains.begin(&cb);
  sprintf_P(topic_status, "curtains/%s/status", sensorid);
  sprintf_P(topic_set, "curtains/%s/set", sensorid);
  sprintf_P(topic_set_all, "curtains/all/set");
  Serial.println("Done.");
#endif

#if (DEVICE_TYPE == TYPE_BUTTON)
  // Init push button
  Serial.println("Init push button...");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), btn_callback, FALLING);
  sprintf_P(topic_status, "button/%s/status", sensorid);
  Serial.println("Done.");
#endif

  // Init networking
  Serial.println("Init networking...");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);
  Serial.print("Sensor publishes to:\n\t");
  Serial.println(topic_status);
#ifdef IS_SUBSCRIBER
  Serial.print("Sensor subscribes to:\n\t");
  Serial.println(topic_set);
  Serial.print("\t");
  Serial.println(topic_set_all);
#endif
  Serial.println("Done.");

  Serial.println("Setup complete.");
}

/* Main loop
 */
void loop() {
  // keep the communictation going
  if (!client.connected()) {
    mqtt_reconnect();
  }
  client.loop();
  resolver.loop();

#if (DEVICE_TYPE == TYPE_BUTTON)
  if(btn_pressed){
    btn_pressed = false;
    sprintf_P(message_status, "toggled");
    // publish sensor status
    client.publish(topic_status, message_status, true);
  }
#endif

#if (DEVICE_TYPE == TYPE_CURTAINS)
  if(event){
    event = false;
    switch(curtains.getStatus()){
      case CURTAIN_UP: sprintf_P(message_status, "up"); break;
      case CURTAIN_DOWN: sprintf_P(message_status, "down"); break;
      default: sprintf_P(message_status, "error"); break; // we don't expect anything else
    }
    // publish sensor status
    client.publish(topic_status, message_status, true);
  }
#endif

  // handle set message
  if(set_received){
    // reset flag
    set_received = false;
#if (DEVICE_TYPE == TYPE_FIRE_PLACE)
    RgbColor rgb;
    // change te color of the fireplace
    if((message_set[0] != '#') || strlen(message_set) != 7){
      Serial.println("Wrong color format");
    }
    else{
      char temp[3] = {message_set[1], message_set[2], '\0'};
      uint8_t r = (uint8_t) strtoul(temp, NULL, 16);
      temp[0] = message_set[3];
      temp[1] = message_set[4];
      uint8_t g = (uint8_t) strtoul(temp, NULL, 16);
      temp[0] = message_set[5];
      temp[1] = message_set[6];
      uint8_t b = (uint8_t) strtoul(temp, NULL, 16);
      unsigned int total = r + g + b;
      if((total > COLOR_TRESHOLD) || (total == 0)){
        Serial.println("Color updated.");
        rgb.r = r;
        rgb.g = g;
        rgb.b = b;
        firePlace.setFireColor(rgb);
      }
    }
    rgb = firePlace.getFireColor();
    sprintf_P(message_status, "#%02x%02x%02x", rgb.r, rgb.g, rgb.b);
#endif

#if (DEVICE_TYPE == TYPE_DOOR_LOCK)
    bool msg_correct = false;
    if(strstr(message_set, "open") != NULL){
      doorLock.open();
      msg_correct = true;
    }
    if(strstr(message_set, "closed") != NULL){
      doorLock.close();
      msg_correct = true;
    }
    if(!msg_correct){
      Serial.println("Wrong message format.");
    }
    if(doorLock.status() == LOCK_OPEN){
      sprintf_P(message_status, "open");
    }
    else{
      sprintf_P(message_status, "closed");
    }
#endif

#if (DEVICE_TYPE == TYPE_CURTAINS)
  bool msg_correct = false;
  if(strstr(message_set, "up") != NULL){
    curtains.goUp();
    msg_correct = true;
  }
  if(strstr(message_set, "down") != NULL){
    curtains.goDown();
    msg_correct = true;
  }
  if(!msg_correct){
    Serial.println("Wrong message format.");
  }
  switch(curtains.getStatus()){
    case CURTAIN_UP: sprintf_P(message_status, "up"); break;
    case CURTAIN_DOWN: sprintf_P(message_status, "down"); break;
    case GOING_UP: sprintf_P(message_status, "going up"); break;
    case GOING_DOWN: sprintf_P(message_status, "going down"); break;
    default: sprintf_P(message_status, "error"); break;
  }
#endif

    // publish sensor status
    client.publish(topic_status, message_status, true);
  }

#if PERIODIC_UPDATES
  // perform measurement and publish status
  unsigned long currentTime = millis();
    if(currentTime - lastMeasurement >= MEASUREMENT_PERIOD){
    lastMeasurement = currentTime;

#if (DEVICE_TYPE == TYPE_CO2_SENSOR)
    // measure co2
    uint16_t co2;
    co2 = co2Sensor.getCo2();

    sprintf_P(message_status, "%d", co2);
#endif

#if (DEVICE_TYPE == TYPE_COLOR_SENSOR)
    // read color from color sensor
    RgbColor rgb = colorSensor.getRgb();
    // do some color magic
    printRgb(rgb);
    rgb = EnhanceColor(rgb);
    printRgb(rgb);

    sprintf_P(message_status, "#%02x%02x%02x", rgb.r, rgb.g, rgb.b);
#endif

#if (DEVICE_TYPE == TYPE_LUX_SENSOR)
    // Read lux
    float lux;
    als.getALSLux(lux);
    Serial.print("lux: ");
    Serial.println(lux);

    sprintf_P(message_status, "%4.1f", lux);
#endif

    // publish sensor status
    client.publish(topic_status, message_status, true);
  }
#endif
}
