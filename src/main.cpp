/**
 * Measure and POST to web
 *
 * Takes a measurement by reading an analog pin,
 * POSTs it to ifttt.com maker service,
 * and deep sleeps for a time.
 */
#include "Arduino.h"
#include "esp_deep_sleep.h"
#include <WiFi.h>

// WiFi:
WiFiClient client;
void connectToWiFi(const char * ssid, const char * pwd);
const char * networkName = "";
const char * networkPswd = "";

// IFTTT specific
void postMeasurement(int value);
const char* myKey = "";
const char * httpHost = "maker.ifttt.com";
const int httpPort = 80;
const char * eventName = "";

// board / pin / sleep specs
const int LED_BUILTIN = 5;
const int SOILSENSOR_PIN = 12;
const uint64_t SLEEP_DUR = 1800000000; // 30min * 60s == 1800s

void setup()
{
    Serial.begin(115200);
    pinMode(SOILSENSOR_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    connectToWiFi(networkName, networkPswd);
}

void loop()
{
    // blink led to show we're measuring
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);

    // read sensor and POST to maker service at ifttt.com
    int moistureReading = analogRead(SOILSENSOR_PIN);
    Serial.println(moistureReading);
    postMeasurement(moistureReading);

    // go back to sleep
    esp_deep_sleep(SLEEP_DUR);
}

void connectToWiFi(const char * ssid, const char * pwd)
{
    Serial.println("Connecting to WiFi network: " + String(ssid));
    while (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(ssid, pwd);
        // Blink LED while we're connecting:
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void postMeasurement(int value)  // Built HTTP packet to send to Maker
{
    // Packet will end up looking similar to this:
    //
    //  POST /trigger/soil_measured/with/key/(myKey) HTTP/1.1
    //  Host: maker.ifttt.com
    //  User-Agent: ESP32_Thing
    //  Connection: close
    //  Conent-Type: application/json
    //  Conent-Length: (postData length)
    //
    //  {"value1":val}

    String postData = "{\"value1\":"; // build JSON payload
    if(value) postData.concat(value);
    postData.concat("}");

    String request ="POST /trigger/" + String(eventName) + "/with/key/" + String(myKey) + " HTTP/1.1\r\n" +
                    "Host: maker.ifttt.com\r\n" +
                    "User-Agent: ESP32_Thing\r\n" +
                    "Connection: close\r\n" +
                    "Content-Type: application/json\r\n" +
                    "Content-Length: " + String(postData.length(), DEC) + "\r\n\r\n" +
                    postData + "\r\n";
    Serial.print("POSTING: " + request);

    // TODO check if connection was succesful instead of assuming
    client.connect(httpHost, httpPort);
    client.print(request);

    // wait for a response from the server and print it
    delay(5000);
    if (client.available()) {
        String resp = client.readStringUntil('\r');
        Serial.println(resp);
    } else {
        Serial.println(">>> Client Timeout !");
        client.stop();
    }
}
