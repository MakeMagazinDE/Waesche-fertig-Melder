/***************************************************************************
                            LABOS/1, Version 0.9
                            Kilian Hofmann, Feb. 2020
                     ----- WLAN, Disco, Tools  -----

****************************************************************************/

//  ---------------  WLAN-Verbindung starten

void WiFiSetup()
{

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  timeWLANoff = 0; // WLAN off Timer initialisieren

  Serial.print("Verbindung zum WLAN herstellen...");

  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, PASSWORD);

  WriteRGB_LED(255, 0); // Status LED auf NO Wifi

  while (WiFi.status() != WL_CONNECTED) // Verbindungsaufbau (-versuche)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println(WiFi.localIP());

  // Wichtig: ESP8266 WLAN v.2.5 Beta oder neuer verwenden
  client.setInsecure();

  WriteRGB_LED(0, 255); // Status LED auf "Alles gut"
}

//  --------------- Farbe der WLAN LED setzen

void WriteRGB_LED(int Red, int Green)
{

  analogWrite(IO_RGB_WIFI_NO, Red);
  analogWrite(IO_RGB_WIFI_OK, Green);
}

//   --------------- Begrüßung bitte. Aktivieren/Deaktivieren aller LEDs und des Buzzers nach dem hochfahren.

void WelcomeDisco()
{

  bool state = true;
  int delayTime = 300;

  for (int i = 0; i < 2; i++)
  {
    WriteRGB_LED(255, 0);
    delay(delayTime);
    digitalWrite(IO_LED_WASHER_CASE, state);
    delay(delayTime);
    digitalWrite(IO_LED_WASHER_SENSOR, state);
    delay(delayTime);
    digitalWrite(IO_LED_DRYER_CASE, state);
    delay(delayTime);
    digitalWrite(IO_LED_DRYER_SENSOR, state);
    delay(delayTime);
    if (i < 1)
    {
      for (int a = 0; a < 2; a++)
      {
        digitalWrite(IO_WATER_ALERT, HIGH);
        delay(150);
        digitalWrite(IO_WATER_ALERT, LOW);
        delay(150);
      }
    }
    state = false;
  }
  WriteRGB_LED(0, 0);
}

// String sprintf, selbsgebaut...

String labstrsprintf(String pSrc, String pVal)
{
  char src[500];
  char dest[500];
  char buf[100];

  pSrc.toCharArray(src, pSrc.length() + 1);
  pVal.toCharArray(buf, pVal.length() + 1);
  sprintf(dest, src, buf);
  return String(dest);
}

String GetInfo(String User)
{
  String showtext;

  if (User.isEmpty() or User == "???")
    showtext = labstrsprintf(LAB_WHO_AM_I, LAB_GREETING); // + String("\n");
  else
    showtext = labstrsprintf(LAB_WHO_AM_I, User); // + String("\n");

  showtext += String(LABOS_VERSION) + String("\n\n");
  showtext += String(LAB_ABOUT);

  return showtext;
}
