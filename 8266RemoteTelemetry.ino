#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "Adafruit_Si7021.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Timer.h>  //Obtain zip from https://github.com/JChristensen/Timer anf manually install with arduino IDE

//sensors
Adafruit_BMP280 bme; // temperature and humidity
Adafruit_Si7021 sensor = Adafruit_Si7021(); //temperature and pressure

// a timer to call things at intervals
Timer t;

// WiFi network name and password:
char WifiName[] = "SET YOU WIFI SSID NAME HERE";
char WifiPassword[] = "SET YOUR WIFI PASSWORD HERE";

// The UDP port to send notification on
const int udpPort = 49090;

//The udp library class
WiFiUDP sock;

char* lastWifiStatus = "";
char * getWifiStatus()
{
  switch (WiFi.status()) {
  case     WL_IDLE_STATUS:
    return  "Idle";
    break;
  case  WL_NO_SSID_AVAIL:
    return "No SSID Avail";
    break;
  case WL_SCAN_COMPLETED:
    return "Scan compl";
    break;
  case WL_CONNECTED:
    return "Connected";
    break;
  case WL_CONNECT_FAILED:
    return "Failed";
    break;
  case WL_CONNECTION_LOST:
    return "Conn Lost";
    break;
  case WL_DISCONNECTED:
    return "Disconnect";
    break;
  }
  return "Unknown";
}

void connectWifi()
{
  Serial.println("Connecting wifi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WifiName, WifiPassword);
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
  case WIFI_EVENT_STAMODE_GOT_IP:
    Serial.println("Wifi Connected");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.subnetMask());
    break;
  case WIFI_EVENT_STAMODE_DISCONNECTED:
    Serial.println("Wifi disconncted");
    delay(5000);
    connectWifi();
    break;
  }
}

/*
  SendUDP
*/
int sendUDP(WiFiUDP &sock, char buffer[], int len)
{
  Serial.println("Sending UDP packet");
  // take the local IP address and change the last octet to 255 to obtain a broadcast IP
  IPAddress adr = WiFi.localIP();
  adr[3] = 255;
  
  sock.beginPacketMulticast(adr, udpPort, WiFi.localIP());
  // write each byte of the buffer
  for (int i = 0; i < len; i++)
  {
      sock.write((int)buffer[i]);
  }
  // Ending the packet sensd the datagram to the network
  if (sock.endPacket() == 0) {
      Serial.println("packet FAILED");
      return 1;
  }
  return 0;
};

void sendTelemetry(float temp, float humid, float pres)
{
  Serial.println("Sending UDP");
  char buffer[32] = "TELE"; 
  int cur = 5;

  *((float*)(buffer + cur)) = temp;
  cur += sizeof(float);

  *((float*)(buffer + cur)) = humid;
  cur += sizeof(float);

  *((float*)(buffer + cur)) = pres;
  cur += sizeof(float);

  sendUDP(sock, buffer, cur);
}

void initialiseSensors()
{
  Serial.println("Initialising sensors...");
  if (!bme.begin(0x76, 0x60))
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  }
  else
  {
    Serial.println("Started sensor: BMP280");
  }

  if (!sensor.begin())
  {
    Serial.println("Did not find Si7021 sensor!");
  }
  else
  {
    Serial.println("Started sensor: Si7021");
  }

  Serial.println("Sensor initialisation complete");
}

void getTelemetry()
{
  float values[3] = { 0.0 };
  float temperature, humidity, pressure; 
  temperature = sensor.readTemperature();
  humidity = sensor.readHumidity();
  pressure = bme.readPressure();

  Serial.println("BMP280 results");
  Serial.printf("Temperature = %.2f°C\n", temperature);
  Serial.printf("Pressure = %.2fPa\n", pressure);
  // this should be adjusted to your local pressure reading
  Serial.printf("Approx altitude = %.2fm\n\n", bme.readAltitude(1036));

  Serial.println("SI7021 results");
  Serial.printf("Humidity: %.2f%%\n", humidity);
  Serial.printf("Temperature: %.2f°C\n", bme.readTemperature());

  values[0] = temperature;
  values[1] = humidity;
  values[2] = pressure;

  if (getWifiStatus() == "Connected") {
    sendTelemetry(temperature, humidity, pressure);
  }
}

/*
Setup
*/
void setup(void) {
  Serial.begin(115200);
  Serial.println("Starting");
  initialiseSensors();

  WiFi.onEvent(WiFiEvent);
  connectWifi();
  delay(100);
  t.every(1000 * 10, getTelemetry);
}


/*
Loop
*/
void loop(void) {
  t.update();
}
