#include "OV7670.h"
#include <WiFi.h>
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
#define host "<IP do Servidor>"

OV7670 * camera;

unsigned char bmpHeader[BMP::headerSize];

void send()
{
    WiFiClient client;

    // Flask server running on port 5000 by default.
    if (client.connect(host, 5000)) {
        Serial.println("Start sending POST");
        client.println("POST /detection HTTP/1.1");
        client.println("Host:port");
        client.println("Cache-Control: no-cache");
        client.println("Content-Type: image/bmp");
        client.println();

        client.write(bmpHeader, BMP::headerSize);
        client.write(camera->frame, camera->xres * camera->yres * 2);

        long interval = 5000;
        unsigned long currentMillis = millis(), previousMillis = millis();

        while(!client.available()){

            if( (currentMillis - previousMillis) > interval ){
                Serial.println("Timeout");
                client.stop();     
                return;
            }

            currentMillis = millis();
        }

        while (client.connected())
        {
            if ( client.available() )
            {
                char str = client.read();
                Serial.println(str);
            }
        }
    }
}


void setup()
{
    pinMode(5, INPUT);
    Serial.begin(115200);
    while(!Serial){};
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
    if (digitalRead(5) == 1)
    {
        camera->oneFrame();
        send();
        delay(3000);
    }
}
