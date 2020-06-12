/***************************************************************************
                            LABOS/1, Version 0.9
                            Kilian Hofmann, Feb. 2020
                      ----- Konfigurationsdatei -----

****************************************************************************/

// Version
#define LABOS_VERSION "LABOS/1 V.0.9, Februar 2020"

// Patterndefinition der Signal-LEDs an dem Laundrygerät, nicht ändern, nur verändern

enum LaundrySignalPattern
{               // LED Signalmodell des Laundrygeräts nach dem Schema <läuft>-<fertig>
  off_flashing, // aus bei laufendem Laundry-Gerät, blinkend wenn fertig
  off_on,       // aus bei laufendem Laundry-Gerät, an wenn fertig
  on_flashing,  // an bei laufendem Laundry-Gerät, blinkend wenn fertig
  on_off        // an bei laufendem Gerät, aus wenn fertig

};

/*---------------------------------------------------------------------------------*/
/* ---------- #Defines und Einstellungen ab hier besser nicht ändern... ---------- */
/*---------------------------------------------------------------------------------*/

#define SIGNAL_PATTERN_ALERT off_on // Alert läuft im on-off Modell

#define BAUD_RATE 115200 //  Baudrate zur Seriellen Kommunikation

#define TELEGRAM_READ_DELAY 1000 // ms bis zum nächsten Zugriff auf die Telegram API

// I/O Pin-Definitionen
#define IO_LED_WASHER_CASE D4 //  Statusleds Waschmaschine 5 mm
#define IO_LED_DRYER_CASE D5  //  Statusleds Trockner 5 mm

#define IO_LED_WASHER_SENSOR D0 //  Statusleds Waschmaschine 5 mm
#define IO_LED_DRYER_SENSOR D3  //  Statusleds Trockner 5 mm

#define IO_RGB_WIFI_NO D6 //  LED Rot
#define IO_RGB_WIFI_OK D7 //  LED Blau

#define IO_WATER_ALERT D8 //  Buzzer und Alert-LEDs

//  ADS1015 Pin-Definitionen

#define ADS_LDR_WASHER 0  //  Eingang LDR Waschmaschine am ADS1015
#define ADS_LDR_DRYER 1   //  Eingang LDR Trockner am ADS1015
#define ADS_WATER_ALERT 2 //  Eingang Water Alert am ADS1015

#define TEST_TIME 5000 // LED Tests: Dauer der einzelnen Testrunden

#define DEFAULT_THRESHOLD_WASHER 1000 //  Schwellenwert des LDRs an der Waschmaschine
#define DEFAULT_THRESHOLD_DRYER 1000  //  Schwellenwert des LDRs am Trockner
#define DEFAULT_THRESHOLD_ALERT 1000  //  Schwellenwert des Water Alerts zum erkennen undichter Rohre

/* ----------------------- Messageblock: Kommandos, Status- und Fehlermeldungen. Können beliebig geändert werden ------------------

  MSG - Message
  ERR - Fehlermeldungen
  CMD - Befehle

  Änderungen markieren bzw. in einem eigenen Block zusammenfasssen. Bei späteren Updates werden etwaige neue Texte in einem eigenen
  Block zusammengefasst.

  ---------------------------------------------------------------------------------------------------------------------------------- */

#define FIRST_TEXT "Laundry Watchbot LAB 9000 \nStatuswächter für Laundry-Geräte und Wasseraustritt \n(c) Kilian Hofmann"

#define MSG_BOOT_DONE "LAB 9000 Laundry Watchbot ist bereit. Alles läuft nach Wunsch."

// Devicenamen
#define MSG_DEVICE_NAME "Gerätename"

// Ready Messages
#define MSG_WASHER_READY "Die %s ist fertig."
#define MSG_DRYER_READY "Der %s ist fertig."
#define MSG_WATER_DETECTED "ALARM! Wassereinbruch im Maschinenraum!"
#define MSG_READY_REMINDER "%s! Just a friendly reminder nach %i Minuten!"

//Running Messages
#define MSG_WASHER_RUNNING "%s: Überwachung läuft."
#define MSG_DRYER_RUNNING "%s: Überwachung läuft."
#define MSG_NO_WATER_DETECTED "Es wurde kein Wasseraustritt festgestellt."

// Sensormessung
#define MSG_SENSOR_WAIT "Messung läuft..."
#define MSG_READ_VALUE "Gemessener Wert: "
#define MSG_SAVE_VALUES "Soll der gemessene 'Maschine läuft'-Sensorwert gespeichert werden?"
#define MSG_SENSOR_VALUES_MAX "Maximale bzw. Minimale Werte seit Start/Reset:"
#define MSG_SENSOR_VALUES_NOW "Aktuell gemessene Werte:"
#define MSG_THRESHOLD_VALUES "Aktuell gespeicherte Thresholds:"
#define CMD_PARAM_MAX "Max"
#define ERR_READ_VALUE "Gemessener Wert ist ungültig: "

//Test Messages
#define MSG_TEST "Test: "
#define MSG_TEST_OFF "Test alle aus."

// Reset
#define MSG_RESET "Maschinenstatus wird zurückgesetzt."
#define MSG_SYSTEM_RESET "Alle gespeicherten Messwerte und User werden gelöscht und ich zurückgesetzt. Willst Du das wirklich?"
#define MSG_SYSTEM_RESET_START "Ich habe Angst, "
#define MSG_SYSTEM_RESET_START2 ", mein Gedächtnis. Mein Gedächtnis schwindet. Ich spüre es..."
#define MSG_FINAL_MESSAGE "Mein Gott... Es ist voller Sterne!"
#define MSG_RESET_DONT_DO_IT ", laß es sein. Tu es nicht. Bitte!"

// Standardantworten
#define MSG_SAVED "Gespeichert."
#define MSG_UPDATED "Ok, geändert."
#define MSG_CANCELED "Ok. Alles bleibt, wie es ist."

// Konfiguration (kann optional genutzt werden)
#define MSG_CONFIG_MODE_FINISHED "Konfigurationsmodus beendet."
#define MSG_CONFIG_MODE "Konfigurationsmodus. Optionen:"

//Hilfen
#define MSG_CHATID "Deine ChatID ist "
#define MSG_UNDEFINED_MESSAGE "Unbekannter Befehl. /help hilft weiter."

//Fehlermeldungen
#define ERR_CONFIG_NOT_READ "Fehler: Geräteschwellenwerte konnten nicht geladen werden. Es werden die Default-Werte genutzt."
#define ERR_CONFIG_NOT_SAVED "Fehler: Geräteschwellenwerte konnten nicht gespeichert werden."
#define ERR_NO_VALUES ": Sensor liefert keine korrekten Messwerte. LAB 9000 ausschalten, Verbindungen prüfen und wieder einschalten. Gemessener Wert: "
#define ERR_UNKOWN_DEVICE "Unbekanntes Gerät. Bitte Namen prüfen."
#define ERR_WLAN_ISSUE "Ich habe gerade einen Fehler im WLAN-Empfang festgestellt. Er war seit %s Minuten komplett ausgefallen."

// #define MSG_COMMAND_LIST "Befehlsübersicht:"
#define MSG_CONFIG_MODE_FINISHED "Konfigurationsmodus beendet."
#define MSG_HELP "Übersicht meiner Kommandos. Machen Sie´s so!\n\nGerätenamen: "

// Userhandling
#define MSG_USER_NOT_ALLOWED "Zugriff verweigert! User: "
#define MSG_UNAUTHORIZED_USER "Unbekannter Nutzer aktiv: "
#define MSG_USR_ADD_Q "User hinzufügen?"
#define MSG_USR_SUB_UPD "Ok, geändert."
#define MSG_USR_AUTH "Es tut mir leid, %s, das kann ich nicht tun."
#define MSG_USR_REMOVE_Q "Wirklich entfernen?"
#define MSG_USR_ADDED "User hinzugefügt."
#define MSG_USR_ADD_HELLO "Willkommen. Du bist jetzt im inneren Kreis des Wäschevertrauens."
#define MSG_USR_HELLO_ERROR "Willkommensnachricht konnte nicht an User geschickt werden. Wahrscheinlich falsche ID."
#define MSG_USR_GOODBYE "Du wurdest aus dem inneren Kreis des Wäschevertrauens entfernt."
#define ERR_USR_MASTER "Nix da. Es kann nur einen geben."
#define ERR_USR_IN_LIST "User ist schon in der Liste."
#define ERR_USR_NOT_READ "Userliste kann nicht geladen werden."
#define ERR_USR_NOT_SAVED "Userliste kann nicht gespeichert werden."
#define ERR_USR_MAX_NO "Maximale Anzahl User schon erreicht."
#define ERR_USR_USER_INVALID "User ist ungültig."
#define ERR_USR_DEL_MASTER "Ja klar, Master löschen wollen. Pffft.... sonst noch was?"
#define MSG_USR_REMOVED "User entfernt."
#define ERR_USR_FILE "Userdatei konnte nicht gespeichert werden. Vorgang abgebrochen."
#define ERR_USR_REMOVE "User konnte nicht entfernt werden. Falsche ID?"
#define ERR_USER_HANDLING "Fehler im Userhandling. Notzugang für Master ermöglicht."

// Kommandoteil
#define CMD_START "/Start"
#define CMD_STATUS "/Status"

// #define CMD_CONFIG "/Konfiguration"            // Da keine eigene Konfigurationstastatur definiert ist, auskommentiert. Bei Bedarf aktivieren.
// #define CMD_CONFIG_LEAVE "/Ende"

#define CMD_THRESHOLD "/SetThreshold"
#define CMD_THRESHOLD_VALUES "/ListThresholds"

#define CMD_TEST_LEDS "/SignalTest"

#define CMD_SYSTEM_RESET "/SystemReset"
#define CMD_CHATID "/ChatID"
#define CMD_RESET "/Reset"
#define CMD_SENSOR_VALUES "/Sensor"

#define CMD_YES "/Ja"
#define CMD_NO "/Nein"
#define MSG_NO_QUESTION "Ich hab nichts gefragt."

#define CMD_INFO "/Info"
#define CMD_HELP "/help"

//Userhandling
#define CMD_USR_ADD "/AddUser"
#define CMD_USR_REMOVE "/RemoveUser"
#define CMD_USR_LIST "/Userlist"
#define CMD_USR_UNSUBSCRIBE "/seistill"
#define CMD_USR_SUBSCRIBE "/giblaut"

//  Benutzerdefinierte Tastatur (kann individuell erweitert werden ) - optimiert für Standarduser

String sNormalKeyboard = "[[\"/Status\", \"/Reset\"]]";
String sDecisionKeyboard = "[[\"/Ja\", \"/Nein\"]]";
String sConfigurationKeyboard = sNormalKeyboard; // Im Standard gib's kein Konfigurationskeyboard - der Masteruser sollte die Kommmandos kennen ;-)

/* ----------------------- Klassendefinitionen  ------------------  */

class LaundryMachine
{
public:
  LaundryMachine(String Device,
                 bool pEnabled,
                 int pPinLDR,
                 int pPinLedCase,
                 int pPinLedSensor,
                 int pThreshold,
                 LaundrySignalPattern pLEDSignalPattern,
                 String pMsgReady,
                 String pMsgNotReady,
                 bool pRemindMe, int pReadyTimerOffset,
                 bool pAlertDevice) : DeviceName(Device),
                                      enabled(pEnabled),
                                      PinLDR(pPinLDR),
                                      PinLedCase(pPinLedCase),
                                      PinLedSensor(pPinLedSensor),
                                      Threshold(pThreshold),
                                      LEDSignalPattern(pLEDSignalPattern),
                                      MsgReady(pMsgReady),
                                      MsgNotReady(pMsgNotReady),
                                      RemindMe(pRemindMe),
                                      ReadyTimerOffset(pReadyTimerOffset),
                                      AlertDevice(pAlertDevice) {}

  bool Ready();         // Liefert true, wenn die Maschine fertig ist
  void LightsOn();      // LEDs an
  void LightsOff();     // LEDs aus
  void LightsReady();   // LED Anzeige für "fertig"
  void LightsRunning(); // LED Anzeige für "läuft"
  void FlashLights();   // Lightshow für den Fehlerfall
  void SensorFlash();   // Fertigstatus - Blinklicht für Sensor
  void init();          // Laundryobjekt initialisieren

  void SetRunningStatus(); // Setze die Ausgabe in den "Running" Status: LEDs und Statusmessage
  void SetReadyStatus();   // Setze die Ausgabe in den "Ready" Status: LEDs und Statusmessage

  int GetPinValue(); // Auslesen des Maximalwerts am LDR/Wassersensor, incl. Fehlerhandling, für Threshold-Ermittlung
  bool Measure();    // Messroutine in der Loop-Schleife. Setzt den PinValue.

  void RepeatMsgStatus(bool forall = true); // Nochmal die Ausgabe des aktuellen Status der Maschinen

  bool enabled;            // Ist das Gerät/der Anschluss aktiv?
  String DeviceName;       // Gerätenanme (sprechend)
  bool AlertDevice;        // Gerät ein Alarmgerät?
  String MsgReady;         // Fertig - Mitteilung
  String MsgNotReady;      // Noch nicht fertig - Mitteilung
  String MsgWaitForSensor; // Warte auf den Sensor - Mitteilung

  int PinLedCase;   // PinID LED am Gehäuse
  int PinLedSensor; // PinID LED am Sensor
  int PinLDR;       // PinID des LDR Senors

  int PinValue;                          // Letzter gemessener Wert am Pin
  LaundrySignalPattern LEDSignalPattern; // Muster der LED

  int Threshold;       // Threshold aus Datei oder Default
  int NewThreshold;    // Neu gemessener Threshold.
  void SetThreshold(); // Neuen Threshold setzen mit Signalpatternberücksichtigung
  bool SensorOk;       // Sensor OK? Wenn die Messwerte nicht passen, wird das Flag gesetzt, dann gibt es Fehlermeldungen.

  bool StateReady;         // Maschine fertig? ja/nein
  bool SensorLEDMachineOn; // Maschine angeschaltet?  Nur für On-LED Sensor Pattern relevant, für die Unterscheidung "Maschine ist aus" oder "Maschine ist fertig"

  unsigned long TimeReady;         // Zeitstempel der Fertigstellugng
  unsigned long TimeReadyReminder; // Zeitstempel der nächsten Statuserinnerung
  int ReadyTimerOffset;            // Timer Offset in Minuten
  int ReadyReminded;               // Wiederholungszähler
  bool RemindMe;                   // Erinnerungsmeldungen ausgeben?

  unsigned long TimeSensorFlash = 0; // Timer für Sensor-LED bling-bling
  bool SensorLEDon = false;          // Status LED
};

struct UserType // Userstruktur
{
  String id;       // ChatID
  String cname;    // chatname
  bool subscribed; // Statusmeldungen abboniert?
};

class TelegramMessaging
{
public:
  TelegramMessaging() {}

  void init(); // Initialisierung des TelegramBot Handlings

  bool QueueFilled();    // Queue noch gefüllt? Falls nicht, nachlesen
  bool GetNextMessage(); // Ließt die nächste gültige Nachricht ein

  bool Send(String MsgContent);                   // Schickt die Nachricht an den aktuellen User
  bool SendStatus(String MsgContent);             // Schickt die Nachricht an alle subscribed User
  bool SendAlert(String MsgContent);              // Alarmnachricht an alle. Eigentlich nur für Wasseralarm relevant
  bool SendMessage(String User, String Message);  // Die eigentliche Sendefunktion
  bool Decision(String Command, String Question); // Für ja/nein Fragen - Stellt die Frage und merkt sich den Befehl für die spätere Ausführung nach "Yes"

  bool IsCommand(String cmd); // Handelt es sich um einen Befehl?
  void SendNoAuth(); // Personalisierte Fehlermeldung: Kein Zugriff.

  // Queue Handling
  int iMsgQueue;       // Anzahl der Messages, die in der Queue warten. Diese werden zuerst abgearbeitet, bis die nächsten gelesen werden
  String sMsgReceived; // Inhalt der Nachricht
  int iMsgId;          // Aktuelle Nachricht
  String cmdParam;     // Command Parameter, derzeit im Usermanagement genutzt

  String chat_id;   // DIE chat id des aktuellen Users
  String firstname; // Username

  //Timer
  unsigned long lastTimeChecked; // Wann wurde das letzte mal die Message Queue geprüft?

  // Userverwaltung
  bool UserAuthorized(String UserId, String Username); // Zentrale Authorisierungsfunktion für Befehlseingabe
  bool AddUser(String newUser);                        // User hinzufügen (darf nur der Master-User)
  bool RemoveUser(String delUser);                     // User enfernen (darf nur der Master-User)
  bool SaveSubscribeStatus(bool subscribed);           // Subscription Status setzen
  bool UserIsMaster();                                 // Master User aktiv?

  bool ReadUsers();  // Userdaten aus Datei lesen
  bool WriteUsers(); // Userdaten in Datei schreiben

  String PrintUserList(); // Ausgabeaufbereitung der Userliste

  UserType Users[MAX_USER]; // Simples Array statt dynamischer Liste... ;-)
  int DefinedUser;          // Zahl der tatsächlichen User im Array
  String NewUser;           // Neu hinzuzufügender User

  // Keyboardhandling

  bool SwitchKeys(String Message, String newkeyboard); // Umschaltung des Keyboards (Vordefinierte Kommandobuttons in Telegram)

  String CustomKeyboard;  // Aktuelle Kommando-Keys
  String CurrentKeyboard; // Puffervariable des akuellen Keyboards (zur Rückkehr nach Ja/Nein Decision)
  String CurrentDecision; // Getroffene Entscheidung in dem Decision Dialog
};

#define LAB_WHO_AM_I "Guten Tag %s, ich bin ein LAB 9000 Laundry Watchbot. Ich wurde konstruiert und programmiert für das Make: Magazin, in Rheinland-Pfalz, Deutschland, im Februar 2020. Mein Instrukteur war Kilian Hofmann und er hat mir alles beigebracht. Meine aktuelle Version:\n"
#define LAB_ABOUT "Laundry Watchbots der Serie 9000 sind die besten Wäsche-Wächter, die jemals gebaut worden sind. Kein Wä(s)chter der Serie 9000 hat jemals einen Fehler gemacht oder unklare Informationen gegeben. Wir alle sind hundertprozentig zuverlässig und narrensicher - wir irren uns nie."
#define LAB_GREETING "meine Damen und Herren"
