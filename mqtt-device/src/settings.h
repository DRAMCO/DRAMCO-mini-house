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
 *         File: settings.h
 *      Created: 2019-01-30
 *       Author: Geoffrey Ottoy
 *
 *  Description: Device settings (device type, device id, mqqt server, etc.)
 * 
 *  License: GNU GPL v3.0 (see LICENSE)
 */


#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// DON'T CHANGE ///////////////////////////////////////////////////////////////
#define TYPE_DOOR_LOCK    1
#define TYPE_CO2_SENSOR   2
#define TYPE_COLOR_SENSOR 3
#define TYPE_LUX_SENSOR   4
#define TYPE_FIRE_PLACE   5
#define TYPE_CURTAINS     6
#define TYPE_BUTTON       7
// DON'T CHANGE ///////////////////////////////////////////////////////////////

//#define DEVICE_TYPE TYPE_DOOR_LOCK
//#define DEVICE_TYPE TYPE_CO2_SENSOR
//#define DEVICE_TYPE TYPE_COLOR_SENSOR
//#define DEVICE_TYPE TYPE_LUX_SENSOR
//#define DEVICE_TYPE TYPE_FIRE_PLACE
//#define DEVICE_TYPE TYPE_CURTAINS
#define DEVICE_TYPE TYPE_BUTTON

#define MEASUREMENT_PERIOD  1000

#define MAX_MESSAGE_LEN 16
#define MAX_TOPIC_LEN 64

const char* ssid = "DRAMCO-SMIoT-Gateway";
const char* password = "SMIoT2019";
const char* mqtt_name = "dramco-mqtt-server.local";

//const char* sensorid = "doorlock-10cb";
//const char* sensorid = "co2sensor-0c02";
//const char* sensorid = "colorsensor-8421";
//const char* sensorid = "luxsensor-4f23";
//const char* sensorid = "fireplace-f17e";
//const char* sensorid = "curtains-09e4";
const char* sensorid = "button-9b65";

#endif
