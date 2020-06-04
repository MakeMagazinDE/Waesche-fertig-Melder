/***************************************************************************
                            LABOS/1, Version 0.9
                            Kilian Hofmann, Feb. 2020
                ----- setup(), loop() und Command-Verarbeitung -----

****************************************************************************/

#include <ESP8266WiFi.h>          //  WiFi-Standardbibliothek für den ESP8266
#include <WiFiClientSecure.h>     //  sichere WiFi-Verbindung, Teil der ESP8266WiFi Bibiliothek
#include <UniversalTelegramBot.h> //  UniversalTelegramBot-Library von Brian
#include <ArduinoJson.h>          //  Json-Library für Arduino; benötigt für UniversalTelegramBot.h; Wichtig: Version 5.13.4 verwenden
#include <Wire.h>                 //  I2C-Kommunikation mit dem ADS1015
#include <Adafruit_ADS1015.h>     //  Library für den ADS1015 D/A Wandler
#include "FS.h"                   //  Flash-Speicher (SPIFFS), verhält sich prinzipiell wie ein EEPROM

#include "LABOS_Setup.h"     // Individuelle Einstellungen, die je Installation geändert werden müssen - WLAN, Token usw.
#include "a_configuration.h" // Laundry Watchbot Konfigurationsdatei

WiFiClientSecure client;                              //  Erzeugung einer sicheren WiFi-Verbindung
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client); //  Telegram-Bot starten
Adafruit_ADS1015 ads;                                 //  Das für die 12-bit Version verwenden; Default-Adresse: 0x48

// ------------------- Start Laundry Watchbot Coding -------------------------------------

unsigned long timeWLANoff; // Startzeit, bei der das WLAN ausgefallen ist

TelegramMessaging Msg; // Message-Klasse für Telegram-Handling

// Laundry-Objekte

LaundryMachine Washer(WASHER_DEVICE_NAME,              // Gerätename
                      WASHER_ENABLED,                  // Soll die Laundry Maschine überhaupt überwacht werden?
                      ADS_LDR_WASHER,                  // LDR-Pin
                      IO_LED_WASHER_CASE,              // LED im Gehäuse
                      IO_LED_WASHER_SENSOR,            // LED am Sensor
                      DEFAULT_THRESHOLD_WASHER,        // Default Threshold des Sensors
                      SIGNAL_PATTERN_WASHER,           // Signalpatten der LED
                      MSG_WASHER_READY,                // Message für fertige Waschmaschine
                      MSG_WASHER_RUNNING,              // Message für laufende Waschmaschine
                      WASHER_REMIND_ME,                // Erinnerungsfunktion aktiv? Beim WaterAlert immer true
                      LAUNDRY_REMINDER_OFFSET_MINUTES, // Erinnerungsoffset in Minuten
                      false);                          // Alert-Device ist hier nur der WaterAlert?

LaundryMachine Dryer(DRYER_DEVICE_NAME,
                     DRYER_ENABLED,
                     ADS_LDR_DRYER,
                     IO_LED_DRYER_CASE,
                     IO_LED_DRYER_SENSOR,
                     DEFAULT_THRESHOLD_DRYER,
                     SIGNAL_PATTERN_DRYER,
                     MSG_DRYER_READY,
                     MSG_DRYER_RUNNING,
                     DRYER_REMIND_ME,
                     LAUNDRY_REMINDER_OFFSET_MINUTES,
                     false);

LaundryMachine WaterAlert(ALERT_DEVICE_NAME,
                          ALERT_ENABLED,
                          ADS_WATER_ALERT,
                          IO_WATER_ALERT,
                          IO_WATER_ALERT,
                          DEFAULT_THRESHOLD_ALERT,
                          SIGNAL_PATTERN_ALERT,
                          MSG_WATER_DETECTED,
                          MSG_NO_WATER_DETECTED,
                          true,
                          ALERT_REMINDER_OFFSET_MINUTES,
                          true);

/* ---------------------------------------------------------------------------------
     Setup
  --------------------------------------------------------------------------------- */

void setup()
{

  Serial.begin(BAUD_RATE); // Für Kommunikation mit dem seriellen Monitor --> Debugging
  ads.begin();             //  ADS Bausteininitialisierung
  SPIFFS.begin();          // Filehandling Initialisierung

  //  Initialisierung der Laundry Objekte
  Washer.init();
  Dryer.init();
  WaterAlert.init();

  ReadThresholds(); //  Thresholds für LaundryMachines aus config-Datei laden

  pinMode(IO_RGB_WIFI_NO, OUTPUT); // PinMode für WiFi Anzeige LED, wird für die WelcomeDisco VOR WifiSetup benötigt
  pinMode(IO_RGB_WIFI_OK, OUTPUT);

  WelcomeDisco(); // Light- und Peepshow zum Start
  WiFiSetup();    //  WiFi initialisieren und starten
  Msg.init();     // Msg.Initialisieren, User laden

  Msg.SendStatus(String(MSG_BOOT_DONE)); // + String(LABOS_VERSION)); // Bereit-Meldung, optional mit Versionsangabe
}

/* ---------------------------------------------------------------------------------
     Main loop
  --------------------------------------------------------------------------------- */

void loop()
{
  String showtext;
  Serial.println("Loop: iMsgQueue " + String(Msg.iMsgQueue) + ", iMsgId " + String(Msg.iMsgId));

  if (Msg.QueueFilled()) // Wenn in der Message Queue noch was drin ist...
  {
    Washer.SensorFlash();
    Dryer.SensorFlash();

    while (Msg.GetNextMessage())
    {
      Washer.SensorFlash(); // Sorgt fürs Blinken der Status LEDs am Sensor
      Dryer.SensorFlash();
      CheckMessages(); // und arbeite sie ab.

    } // ermittle die nächste gültige Message...
  }

  // Keine Nachrichten mehr? Dann starte Messzyklus
  for (int i = 0; i <= ADS_LDR_MAX_READ_SECONDS * 20; i++)
  {

    Washer.Measure();
    Dryer.Measure();
    WaterAlert.Measure();

    Washer.SensorFlash();
    Dryer.SensorFlash();

    delay(50);
  }

  // Prüfe Ergebnisse

  if (Washer.Ready())
    Washer.SetReadyStatus();
  if (Dryer.Ready())
    Dryer.SetReadyStatus();
  if (WaterAlert.Ready())
    WaterAlert.SetReadyStatus();

  // Sensor-Fehler? Dann blinken (wird hier zusammengefasst, damit die Delays nicht im Messzyklus die funktionierenden Sensoren stören)
  if (!Washer.SensorOk)
    Washer.FlashLights();
  if (!Dryer.SensorOk)
    Dryer.FlashLights();
  if (!WaterAlert.SensorOk)
    WaterAlert.FlashLights();

  // WLAN noch da?
  while (WiFi.status() != WL_CONNECTED)
  { // Verbindungsaufbau (-versuche)
    Serial.print(".");
    WriteRGB_LED(0, 0);
    delay(500);
    WriteRGB_LED(255, 0);
    delay(500);
    if (timeWLANoff == 0)
      timeWLANoff = millis();
  }

  if (timeWLANoff > 0) // War WLAN weg? Dann Nachricht an den Master.
  {
    int diff;
    String showtext;
    diff = int((millis() - timeWLANoff) / 60000);
    Serial.println("WLAN off for " + String(diff) + " Minutes.");
    if (diff > 5)
      showtext = labstrsprintf( ERR_WLAN_ISSUE, String (diff));
      // Msg.SendMessage(MASTER_USER, String(diff) + String(ERR_WLAN_ISSUE)); // Ausgabe der Netto-Offzeit, allerdings nur wenn das Netz länger als 5 Minuten weg war
      Msg.SendStatus(showtext);
    timeWLANoff = 0;
    WriteRGB_LED(0, 255);
  }
}

/* ---------------------------------------------------------------------------------
     Prüfung aller Messages aus Telegram - und Execution
  --------------------------------------------------------------------------------- */

void CheckMessages()
{
  String showtext;

  // ---------------- Statusausgabewiederholung  der Sensoren  --------------------

  if (Msg.IsCommand(CMD_STATUS))
  {

    Washer.RepeatMsgStatus(false); // optionaler Parameter false: Nur für den aktuellen User, nicht für alle subscriber
    Dryer.RepeatMsgStatus(false);
    WaterAlert.RepeatMsgStatus(false);
  }

  // --------------  Reset Maschinenstatus, relevant wenn eine Maschine fertig ist bzw. bei Alarm

  else if (Msg.IsCommand(CMD_RESET))
  {

    Msg.Send(MSG_RESET);

    Washer.SetRunningStatus();
    Dryer.SetRunningStatus();
    WaterAlert.SetRunningStatus();
  }

  // -------------- Chat ID Abfrage  --------------------------

  //  Ausgabe der aktuellen ChatId für optionale Zugriffssteuerung - Testversion
  else if (Msg.IsCommand(CMD_CHATID))
  {

    //  Wert ermitteln
    Msg.Send(MSG_CHATID + Msg.chat_id);
  }

  // --------------  Thresholds setzen   --------------------------

  else if (Msg.IsCommand(CMD_THRESHOLD))
  {

    if (Msg.UserIsMaster()) // Neue Thresholds darf nur der Masteruser speichern
    {
      if (Msg.cmdParam.equalsIgnoreCase(WASHER_DEVICE_NAME) and Washer.enabled)
      {
        Msg.Send(MSG_SENSOR_WAIT);

        Washer.NewThreshold = Washer.GetPinValue();
        if (Washer.NewThreshold > 0)
        {
          Msg.Send(MSG_READ_VALUE + String(Washer.NewThreshold));
          Msg.Decision(CMD_THRESHOLD, MSG_SAVE_VALUES);
        }
        else
          Msg.Send(ERR_READ_VALUE + String(WaterAlert.NewThreshold));
      }
      else if (Msg.cmdParam.equalsIgnoreCase(DRYER_DEVICE_NAME) and Dryer.enabled)
      {
        Msg.Send(MSG_SENSOR_WAIT);
        Dryer.NewThreshold = Dryer.GetPinValue();

        if (Dryer.NewThreshold > 0)
        {
          Msg.Send(MSG_READ_VALUE + String(Dryer.NewThreshold));
          Msg.Decision(CMD_THRESHOLD, MSG_SAVE_VALUES);
        }
        else
          Msg.Send(ERR_READ_VALUE + String(Dryer.NewThreshold));
      }
      else if (Msg.cmdParam.equalsIgnoreCase(ALERT_DEVICE_NAME) and WaterAlert.enabled)
      {
        Msg.Send(MSG_SENSOR_WAIT);
        WaterAlert.NewThreshold = WaterAlert.GetPinValue();

        if (WaterAlert.NewThreshold > 0)
        {
          Msg.Send(MSG_READ_VALUE + String(WaterAlert.NewThreshold));
          Msg.Decision(CMD_THRESHOLD, MSG_SAVE_VALUES);
        }
        else
          Msg.Send(ERR_READ_VALUE + String(WaterAlert.NewThreshold));
      }
      else
      {
        Msg.Send(ERR_UNKOWN_DEVICE);
        Msg.cmdParam = "";
      }
    }
    else
      Msg.SendNoAuth();
  }

  // --------------  Userhandling   --------------------------

  // ---------- - Add User --------------

  else if (Msg.IsCommand(CMD_USR_ADD))
  { // Darf nur der Master-User

    if (Msg.UserIsMaster())
    {
      if (Msg.cmdParam.isEmpty()) // Kleine Voarbprüfung
        Msg.Send(ERR_USR_USER_INVALID);
      else
        Msg.Decision(CMD_USR_ADD, MSG_USR_ADD_Q);
    }
    else
      Msg.SendNoAuth();
  }

  // ---------- - Remove  User --------------

  else if (Msg.IsCommand(CMD_USR_REMOVE))
  { // Darf nur der Master-User

    if (Msg.UserIsMaster())
    {
      if (Msg.cmdParam.isEmpty())
        Msg.Send(ERR_USR_USER_INVALID);
      else
        Msg.Decision(CMD_USR_REMOVE, MSG_USR_REMOVE_Q);
    }
    else
      Msg.SendNoAuth();
  }

  // ---------- - Subscribe --------------

  else if (Msg.IsCommand(CMD_USR_SUBSCRIBE))
  {

    if (Msg.SaveSubscribeStatus(true))
      Msg.Send(MSG_UPDATED);
  }

  // ---------- - UnSubscribe --------------

  else if (Msg.IsCommand(CMD_USR_UNSUBSCRIBE))
  {

    if (Msg.SaveSubscribeStatus(false))
      Msg.Send(MSG_UPDATED);
  }

  // ---------- - List Users --------------

  else if (Msg.IsCommand(CMD_USR_LIST))
  {

    if (Msg.UserIsMaster()) // Darf nur der Master-User
      Msg.Send(Msg.PrintUserList());
    else
      Msg.SendNoAuth();
  }

  // --------------  Komfortfunktionen   -------------------------------

  // -------------- Kompletter System Reset   --------------------------

  else if (Msg.IsCommand(CMD_SYSTEM_RESET))
  {

    if (Msg.UserIsMaster()) // Darf nur der Master-User
    {
      Msg.Send(Msg.firstname + MSG_RESET_DONT_DO_IT);
      Msg.Decision(CMD_SYSTEM_RESET, MSG_SYSTEM_RESET);
    }
    else
      Msg.SendNoAuth();
  }

  // ---------------- Ausgabe Infoblock -------------------------------------------
  else if (Msg.IsCommand(CMD_START) or Msg.IsCommand(CMD_INFO))
  {

    Msg.Send( GetInfo(Msg.firstname));
  }
  // ---------------- Ausgabe Hilfe -------------------------------------------

  else if (Msg.IsCommand(CMD_HELP))
  {

    showtext = String(MSG_HELP) + String(WASHER_DEVICE_NAME) + String(", ") + String(DRYER_DEVICE_NAME) + String(", ") + String(ALERT_DEVICE_NAME) + String("\n\n");
    showtext += String(CMD_STATUS) + String("\n") + String(CMD_RESET) + String("\n") +  String(CMD_INFO) +  String("\n\n");
    showtext += String(CMD_USR_SUBSCRIBE) + String("\n") + String(CMD_USR_UNSUBSCRIBE) + String("\n\n");
    showtext += String(CMD_CHATID) + String("\n")+ String(CMD_TEST_LEDS) + String(" <") + String(MSG_DEVICE_NAME) + String("> \n");
    showtext += String(CMD_SENSOR_VALUES) + String(" <") + String(MSG_DEVICE_NAME) + String("> | ") + String(CMD_PARAM_MAX) + String("\n\n");
    showtext += String(CMD_THRESHOLD) + String(" <") + String(MSG_DEVICE_NAME) + String("> \n");
    showtext += String(CMD_THRESHOLD_VALUES) + String("\n\n");
    showtext += String(CMD_USR_ADD) + String("\n");
    showtext += String(CMD_USR_REMOVE) + String("\n") + String(CMD_USR_LIST) + String("\n\n");
    showtext += String(CMD_SYSTEM_RESET) + String("\n");

    Msg.Send(showtext);
  }

  // ---------------- Sensorwerte ausgeben-------------------------------------------

  else if (Msg.IsCommand(CMD_SENSOR_VALUES))
  {
    if (Msg.UserIsMaster())
    {
      // bisher ermittelte Maximal-/Minimalwerte - je nach Signalpattern - ausgeben
      if (Msg.cmdParam.equalsIgnoreCase(CMD_PARAM_MAX))
      {
        showtext = String(MSG_SENSOR_VALUES_MAX) + String("\n");
        showtext += String(WASHER_DEVICE_NAME) + String(": ") + String(Washer.PinValue) + String("\n");
        showtext += String(DRYER_DEVICE_NAME) + String(": ") + String(Dryer.PinValue) + String("\n");
        showtext += String(ALERT_DEVICE_NAME) + String(": ") + String(WaterAlert.PinValue) + String("\n\n");
      }
      else
        // Aktuell alle Werte oder nur die für ein Gerät ermitteln
      {
        Msg.Send(MSG_SENSOR_WAIT);
        showtext = String(MSG_SENSOR_VALUES_NOW) + String("\n");
        if (Msg.cmdParam.equalsIgnoreCase(WASHER_DEVICE_NAME) or Msg.cmdParam.isEmpty())
          showtext += String(WASHER_DEVICE_NAME) + String(": ") + String(Washer.GetPinValue()) + String("\n");
        if (Msg.cmdParam.equalsIgnoreCase(DRYER_DEVICE_NAME) or Msg.cmdParam.isEmpty())
          showtext += String(DRYER_DEVICE_NAME) + String(": ") + String(Dryer.GetPinValue()) + String("\n");
        if (Msg.cmdParam.equalsIgnoreCase(ALERT_DEVICE_NAME) or Msg.cmdParam.isEmpty())
          showtext += String(ALERT_DEVICE_NAME) + String(": ") + String(WaterAlert.GetPinValue()) + String("\n\n");
        if (showtext == String(MSG_SENSOR_VALUES_NOW) + String("\n")) // Falscher Gerätename? Fehlermeldung
          showtext = ERR_UNKOWN_DEVICE;
      }
      Msg.Send(showtext);
    }
    else
      Msg.SendNoAuth();

    Msg.cmdParam = "";
  }

  // ---------------- Thresholdwerte ausgeben-------------------------------------------

  else if (Msg.IsCommand(CMD_THRESHOLD_VALUES))
  {
    if (Msg.UserIsMaster())
    {
      showtext = String(MSG_THRESHOLD_VALUES) + String("\n");
      showtext += String(WASHER_DEVICE_NAME) + String(": ") + String(Washer.Threshold) + String("\n");
      showtext += String(DRYER_DEVICE_NAME) + String(": ") + String(Dryer.Threshold) + String("\n");
      showtext += String(ALERT_DEVICE_NAME) + String(": ") + String(WaterAlert.Threshold) + String("\n\n");
      Msg.Send(showtext);
    }
    else
      Msg.SendNoAuth();
  }

  // --------------  LED und Alert Test kombiniert  --------------------------

  else if (Msg.IsCommand(CMD_TEST_LEDS))
  {

    if (Msg.UserIsMaster())
    {

      if (Msg.cmdParam.isEmpty() or Msg.cmdParam.equalsIgnoreCase(WASHER_DEVICE_NAME) or Msg.cmdParam.equalsIgnoreCase(DRYER_DEVICE_NAME) or Msg.cmdParam.equalsIgnoreCase(ALERT_DEVICE_NAME))
      {
        Msg.Send(MSG_TEST_OFF);
        Washer.LightsOff();
        Dryer.LightsOff();
        WaterAlert.LightsOff();
        delay(TEST_TIME * 0.5);

        if (Msg.cmdParam.isEmpty() or Msg.cmdParam.equalsIgnoreCase(WASHER_DEVICE_NAME))
        {
          Msg.Send(MSG_TEST + String(WASHER_DEVICE_NAME));
          Washer.LightsOn();
          delay(TEST_TIME);
          Washer.LightsOff();
        }

        if (Msg.cmdParam.isEmpty() or Msg.cmdParam.equalsIgnoreCase(DRYER_DEVICE_NAME))
        {
          Msg.Send(MSG_TEST + String(DRYER_DEVICE_NAME));
          Dryer.LightsOn();
          delay(TEST_TIME);
          Dryer.LightsOff();
        }

        if (Msg.cmdParam.isEmpty() or Msg.cmdParam.equalsIgnoreCase(ALERT_DEVICE_NAME))
        {
          Msg.Send(MSG_TEST + String(ALERT_DEVICE_NAME));
          WaterAlert.LightsOn();
          delay(TEST_TIME);
          WaterAlert.LightsOff();
        }

        Msg.cmdParam = "";
        Msg.Send(MSG_TEST_OFF);
        delay(TEST_TIME * 0.5);

        Washer.RepeatMsgStatus();
        Dryer.RepeatMsgStatus();
        WaterAlert.RepeatMsgStatus();
      }
      else
        Msg.Send(ERR_UNKOWN_DEVICE);
    }
    else
      Msg.SendNoAuth();
    Msg.cmdParam = "";
  }

  // --------------  Konfigurationstastatur anzeigen --------------------------
#ifdef CMD_CONFIG // Nur berücksichtigen, wenn das Kommando aktiv ist
  else if (Msg.IsCommand(CMD_CONFIG))
    Msg.SwitchKeys(MSG_CONFIG_MODE, sConfigurationKeyboard); //  Entsprechende Tastatur anzeigen
#endif

  // --------------  Konfigurationsmodus beenden --------------------------
#ifdef CMD_CONFIG_LEAVE
  else if (Msg.IsCommand(CMD_CONFIG_LEAVE))
    Msg.SwitchKeys(MSG_CONFIG_MODE_FINISHED, sNormalKeyboard);
#endif

  /* --------------  Yes/No Behandlung. --------------

    Je nach der aktuellanstehenden Entscheidung (CurrentDecision = letzter Befehl), werden hier bei Zustimmung die Aktivitäten ausgeführt.

    ---------------------------------------------------*/

  else if (Msg.IsCommand(CMD_YES))
  {

    //  -------------- Hard-Reset durchführen --------------
    if (Msg.CurrentDecision == CMD_SYSTEM_RESET)
    {
      Msg.Send(MSG_SYSTEM_RESET_START + Msg.firstname + ". " + Msg.firstname + MSG_SYSTEM_RESET_START2);

      SPIFFS.format(); // Technisch wird hier das Filesystem formatiert. Damit sind alle gespeicherten Daten gelöscht

      Msg.Send(MSG_FINAL_MESSAGE);

      ESP.reset(); // Reset des Boards.
    }

    // ------------- User hinzufügen mit dem aktuell übergebenen Parameter hinter dem ':'
    else if (Msg.CurrentDecision == CMD_USR_ADD)
    {
      if (Msg.AddUser(Msg.cmdParam)) // Fehlerhandling ist in der AddUser Funktion
        Msg.Send(MSG_USR_ADDED);
    }

    // ------------ User hinzufügen mit dem aktuell übergebenen Parameter hinter dem ':'
    else if (Msg.CurrentDecision == CMD_USR_REMOVE)
    {
      if (Msg.RemoveUser(Msg.cmdParam))
        Msg.Send(MSG_USR_REMOVED);
      else
        Msg.Send(ERR_USR_REMOVE);
    }
    // ------------ Neue Schwellenwerte der Sensoren speichern: Washer
    else if (Msg.CurrentDecision == CMD_THRESHOLD)
    {
      if (Msg.cmdParam.equalsIgnoreCase(WASHER_DEVICE_NAME))
        Washer.SetThreshold();
      if (Msg.cmdParam.equalsIgnoreCase(DRYER_DEVICE_NAME))
        Dryer.SetThreshold();
      if (Msg.cmdParam.equalsIgnoreCase(ALERT_DEVICE_NAME))
        WaterAlert.SetThreshold();

      if (SaveNewThresholds())
        Msg.Send(MSG_UPDATED);
    }
    else
    {
      Msg.Send(MSG_NO_QUESTION);
    }

    Msg.cmdParam = "";
    Msg.SwitchKeys("", sConfigurationKeyboard);
  }

  // -------------- Bei "Nein" tue nix.

  else if (Msg.IsCommand(CMD_NO))
  {
    //  Entsprechende Abbruch-Nachricht senden
    Msg.Send(MSG_CANCELED);
    Msg.cmdParam = "";
    Msg.CurrentDecision = "";
    Msg.SwitchKeys("", sConfigurationKeyboard);
  }

  // -------------- Finales else... unbekannter Befehl ---------
  else
  {
    Msg.Send(MSG_UNDEFINED_MESSAGE);
  }
}
