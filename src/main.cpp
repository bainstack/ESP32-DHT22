#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// define the Data-Pin of DHT sensor
#define DHTPIN 25
// our sensor is DHT22 type
#define DHTTYPE DHT22
// create an instance of DHT sensor
DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "SSID";
const char *password = "PASSWORD";

void setup()
{
    Serial.begin(115200);
    delay(4000);

    // connect to wifi
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // initiate dht sensor
    dht.begin();
}

void loop()
{
    // use the functions which are supplied by library.
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t))
    {
        Serial.print(".%");
        return;
    }

    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
        StaticJsonBuffer<200> JSONbuffer; //Declaring static JSON buffer
        JsonObject &tempHum = JSONbuffer.createObject();

        JsonArray &values = tempHum.createNestedArray("values"); //JSON array
        values.add(t);                                           //Add value to array
        values.add(h);                                           //Add value to array

        Serial.print("\n");
        tempHum.printTo(Serial);
        Serial.print("\n");

        char JSONmessageBuffer[200];
        tempHum.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

        HTTPClient http; //Declare object of class HTTPClient

        http.begin("192.168.178.35", 3000, "/sendData");    //Specify request destination
        http.addHeader("Content-Type", "application/json"); //Specify content-type header

        int httpCode = http.POST(JSONmessageBuffer); //Send the request
        String payload = http.getString();           //Get the response payload

        http.end(); //Close connection
    }
    else
    {
        Serial.println("Error in WiFi connection");
    }

    //wait a little because sensor is slow
    delay(60000);
}
