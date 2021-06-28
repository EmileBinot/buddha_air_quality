// Created from example code in these libraries:
// https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
// https://github.com/sparkfun/SparkFun_CCS811_Arduino_Library
#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>

#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>

#define PIN_CCS811_SDA D6
#define PIN_CCS811_SCL D7
#define PIN_CCS811_WAK D8

#define LOG_PERIOD_SEC 10
#define ENV_PERIOD_SEC 10
#define SHORT_PERIOD_SEC 10
#define LONG_PERIOD_SEC 600

// graph will show last ten minutes, one sample every 10 seconds
#define TEN_MINUTES 60
// graph will show last 24 hours, one sample every ten minutes
#define ONE_DAY 288

static CCS811 ccs811( 0x5A );
static BME280 bme280;

#include "data.h"

void HardwareSetup();
void WifiSetup();
void collectData();
void WiFiCheck();

typedef struct {
  uint32_t total;
  uint16_t count;
} averager_t;

static void print(const char* fmt, ...) {
  char buf[256];
  va_list args;
  va_start( args, fmt );
  vsnprintf( buf, sizeof(buf), fmt, args );
  va_end( args );
  Serial.write( buf );
}

// SETUP

void setup() {
  HardwareSetup();
  WifiSetup();
  collectData();
}

void HardwareSetup() {
  // setup the pins
  pinMode( PIN_CCS811_WAK, OUTPUT );
  digitalWrite( PIN_CCS811_WAK, 1 );

  // Enable serial
  Serial.begin(9600);
  print("\nCO2, TVOC, temperature, humidity sensor\n");

  // Enable I2C
  Wire.begin( PIN_CCS811_SDA, PIN_CCS811_SCL );

  // Initialize BME280
  bme280.setI2CAddress(0x76);
  if( bme280.beginI2C() == false ) {
    print("Failed to initialize BME280\n");
    while(1);
  } else {
    print("Found BME280!\n");
  }

  // Initialize CCS811
  digitalWrite( PIN_CCS811_WAK, 0 );
 /*CCS811Core::status returnCode =*/ ccs811.begin();
 /* if( returnCode != CCS811Core::SENSOR_SUCCESS ) {
    print("Failed to initialize CCS811\n");
    while(1);
  } else {
    print("Found CCS811!\n");
  }*/
}

void WifiSetup() {
  delay(5);
  
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void collectData() {
  static averager_t tvoc_avg = {0, 0};
  static averager_t co2_avg = {0, 0};
  static unsigned long second_log = 0;
  static unsigned long second_env = 0;
  static unsigned long second_short = 0;
  static unsigned long second_long = 0;
  static uint16_t tvoc = 0;
  static uint16_t co2 = 0;
  
  if( ccs811.dataAvailable() ) {
    ccs811.readAlgorithmResults();
    uint16_t tvoc = ccs811.getTVOC();
    uint16_t co2 = ccs811.getCO2();
    tvoc_avg.total += tvoc;
    co2_avg.total += co2;
    tvoc_avg.count++;
    co2_avg.count++;
  }

  unsigned long second = millis() / 1000;
  if( (second - second_log) > LOG_PERIOD_SEC ) {
    second_log = second;

    if( tvoc_avg.count > 0 ) {
      tvoc = tvoc_avg.total / tvoc_avg.count;
      tvoc_avg.total = 0;
      tvoc_avg.count = 0;
      print("TVOC: %u\n", tvoc);
    }
    if( co2_avg.count > 0 ) {
      co2 = co2_avg.total / co2_avg.count;
      co2_avg.total = 0;
      co2_avg.count = 0;
      print("CO2: %u\n", co2);
    }
  }
  
  if( (second - second_short) > SHORT_PERIOD_SEC ) {
    second_short = second;
    tenMinutesTVOC[ tenMinuteCounter ] = tvoc;
    tenMinutesCO2[ tenMinuteCounter ] = co2;
    tenMinuteCounter = ++tenMinuteCounter % TEN_MINUTES;
  }

  if( (second - second_long) > LONG_PERIOD_SEC ) {
    second_long = second;
    oneDayTVOC[ oneDayCounter ] = tvoc;
    oneDayCO2[ oneDayCounter ] = co2;
    oneDayCounter = ++oneDayCounter % ONE_DAY;
  }

  if( (second - second_env ) > ENV_PERIOD_SEC ) {
    second_env = second;

    digitalWrite(PIN_CCS811_WAK, 1);
    tempC = bme280.readTempC();
    humidity = bme280.readFloatHumidity();
    digitalWrite(PIN_CCS811_WAK, 0);
    print("Temp: %.2fºC Humidity: %.2f%%\n", tempC, humidity);
    ccs811.setEnvironmentalData(tempC, humidity);
  }  
}

// LOOP

void loop() {
  WiFiCheck();  
  collectData();
}

void WiFiCheck() {
  int wifi_retry = 0;
  while(WiFi.status() != WL_CONNECTED && wifi_retry < 5 ) {
      wifi_retry++;
      Serial.println("WiFi not connected. Try to reconnect");
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      delay(100);
  }
  if(wifi_retry >= 5) {
      Serial.println("\nReboot");
      ESP.restart();
  }
}