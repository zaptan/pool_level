/* 
 * File:   config.h
 * Author: zaptan
 *
 * Created on September 24, 2016, 1:00 PM
 */

#define IO_USERNAME    "your_username"
#define IO_KEY         "your_key"

#define WIFI_SSID       "your_ssid"
#define WIFI_PASS       "your_pass"

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
