void createName()
{
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_BT);
    sprintf(apName, "h4IoT-%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    Serial.print("Device Name: ");
    Serial.println(apName);
}

void setClock()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print(F("Waiting for NTP time sync: "));
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
    }
    Serial.println();
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}

bool wifiFlag = false;
int countWifi = 0;
void wifiSetup()
{
    WiFi.begin(ssid, password);
    Serial.println("Connectig to wifi: ");
    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
    {
        Serial.print(".");
        vTaskDelay(1000);
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        countWifi = 0;
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        setClock();
        wifiFlag = true;
    }
    else
    {
        if (wifiFlag)
        {
            Serial.println("Wifi Disconnected");
            wifiFlag = false;
        }
        countWifi++;
        if (countWifi > 10)
        {
            ESP.restart();
        }
    }
}
TaskHandle_t Taskwifi;
void wifiTask(void *)
{
    while (1)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            wifiSetup();
        }
        vTaskDelay(WIFI_RECONNECT_TIME);
    }
}
void wifiTaskSetup()
{
    xTaskCreatePinnedToCore(
        wifiTask,   /* Task function. */
        "wifiTask", /* name of task. */
        4000,       /* Stack size of task */
        NULL,       /* parameter of the task */
        1,          /* priority of the task */
        &Taskwifi,  /* Task handle to keep track of created task */
        0);
    Serial.println("wifiTask setup done");
}