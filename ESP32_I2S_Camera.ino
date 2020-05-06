#include "OV7670.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "BMP.h"

const int SIOD = 21;
const int SIOC = 22;
const int VSYNC = 34;
const int HREF = 35;
const int XCLK = 32;
const int PCLK = 33;
const int D0 = 27;
const int D1 = 25;
const int D2 = 26;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

#define ssid "Xiaomi"
#define password "mudar@123"
#define serverUrl "https://httpbin.org/post"

OV7670 * camera;

unsigned char bmpHeader[BMP::headerSize];

void send()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "text/plain");  //Specify content-type header
        int httpRespCode = http.POST("POSTING from ESP32");

        Serial.print(httpRespCode);

        http.end();
    }
    else
    {
        Serial.println("Error: No WiFi connection");
    }
}

void setup()
{
    pinMode(5, INPUT);
    Serial.begin(9600);
    Serial.println("Initializing Setup");
    delay(4000);  //Delay needed before calling the WiFi.begin
    WiFi.begin(ssid, password);
    Serial.println("Connecting Wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    Serial.println("WiFi connected");

    camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
    BMP::construct16BitHeader(bmpHeader, camera->xres, camera->yres);
}

void loop()
{
    Serial.println(digitalRead(5));
    if (digitalRead(5) == 1)
    {
        camera->oneFrame();
        send();
    }

    delay(1000);
}
