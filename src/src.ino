#include "src.h"

void setup()
{
    Serial.begin(115200);
    Serial.println();
    createName();
    wifiTaskSetup();
}

long timeGet = millis();
int i = 10;
void loop()
{
    while (!wifiFlag)
    {
        vTaskDelay(1000);
    }
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        client->setCACert(rootCACertificate);
        {
            HTTPClient https;
            Serial.print("[HTTPS] begin...\n");
            timeGet = millis();
            if (https.begin(*client, "https://i4c.org.in/time"))
            { // HTTPS
                Serial.print("[HTTPS] GET...\n");
                int httpCode = https.GET();
                if (httpCode > 0)
                {
                    Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
                    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                    {
                        String payload = https.getString();
                        Serial.println(payload);
                    }
                }
                else
                {
                    Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
                https.end();
            }
            else
            {
                Serial.printf("[HTTPS] Unable to connect\n");
            }
            Serial.printf("Time to execute GET: %d\n", millis() - timeGet);
            timeGet = millis();
            if (https.begin(*client, "https://i4c.org.in/data?mydata=hello" + String(i)))
            {
                https.addHeader("Content-Type", "application/x-www-form-urlencoded");
                Serial.print("[HTTPS] POST...\n");
                String s = " ";
                int httpCode = https.POST(s);
                if (httpCode > 0)
                {
                    Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
                    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                    {
                        String payload = https.getString();
                        Serial.println(payload);
                    }
                }
                else
                {
                    Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
                https.end();
            }
            else
            {
                Serial.printf("[HTTPS] Unable to connect\n");
            }
            Serial.printf("Time to execute POST: %d\n", millis() - timeGet);
        }
        delete client;
    }
    else
    {
        Serial.println("Unable to create client");
    }
    Serial.println("Waiting 10s before the next round...");
    delay(10000);
    i++;
}